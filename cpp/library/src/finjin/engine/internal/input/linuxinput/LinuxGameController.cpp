//Copyright (c) 2017 Finjin
//
//This file is part of Finjin Engine (finjin-engine).
//
//Finjin Engine is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//This Source Code Form is subject to the terms of the Mozilla Public
//License, v. 2.0. If a copy of the MPL was not distributed with this
//file, You can obtain one at http://mozilla.org/MPL/2.0/.


//Includes----------------------------------------------------------------------
#include "FinjinPrecompiled.hpp"
#include "LinuxGameController.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/engine/InputComponents.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>

using namespace Finjin::Engine;


//Local functions---------------------------------------------------------------
static bool IsReallyGameController(const LinuxGameController& gameController)
{
    if (gameController.displayName.find("Transceiver") != Utf8String::npos) //This occurs with Microsoft wireless keyboard
        return false;

    return true;
}

static LinuxGameController* GetGameControllerByDevicePath
    (
    StaticVector<LinuxGameController, GameControllerConstants::MAX_GAME_CONTROLLERS>& gameControllers,
    const Path& devicePath
    )
{
    for (auto& gameController : gameControllers)
    {
        if (gameController.devicePath == devicePath)
            return &gameController;
    }

    return nullptr;
}


//Implementation----------------------------------------------------------------

//LinuxGameController
LinuxGameController::LinuxGameController(Allocator* allocator) : Super(allocator)
{
    this->fd = -1;
    this->index = 0;
    this->isNewConnection = false;
}

void LinuxGameController::SetAllocator(Allocator* allocator)
{
    Super::SetAllocator(allocator);
    
    this->devicePath.SetAllocator(allocator);
}

void LinuxGameController::Reset()
{
    Super::Reset();
    
    this->fd = -1;
    this->devicePath.clear();
    this->index = 0;
    this->state.Reset();
    this->isNewConnection = false;
}

void LinuxGameController::Create(size_t index, int fd, const Path& devicePath)
{
    static const char* correctionName[] = { "none (raw)", "broken line" };
    static const char correctionCoefCount[] = { 0, 4 };

    this->devicePath = devicePath;
    this->fd = fd;

    uint8_t axisCount;
    ioctl(fd, JSIOCGAXES, &axisCount);

    uint8_t buttonCount;
    ioctl(fd, JSIOCGAXES, &buttonCount);
    if (buttonCount < 16)
    {
        //The number returned is sometimes too low. Choose a more reasonable lower limit
        buttonCount = 16;
    }

    js_corr corrections[ABS_MAX + 1];
    ioctl(fd, JSIOCGCORR, &corrections);

    char rawIdentifierString[128];
    if (ioctl(fd, JSIOCGNAME(sizeof(rawIdentifierString)), rawIdentifierString) < 0)
        strcpy(rawIdentifierString, "Generic Game Controller");

    Utf8String identifierString(rawIdentifierString);

    auto safeIdentifierString = InputDeviceUtilities::MakeInputDeviceIdentifier(identifierString);

    this->index = index;

    this->instanceName = identifierString;
    this->instanceName += Convert::ToString(index + 1);

    this->productName = identifierString;
    this->displayName = this->productName;

    this->instanceDescriptor = safeIdentifierString;
    this->instanceDescriptor += "-";
    this->instanceDescriptor += devicePath.c_str();

    this->state.isConnected = true;

    this->productDescriptor = safeIdentifierString;

    //std::cout << "Product desc: " << this->productDescriptor << ", " << devicePath << std::endl;
    //std::cout << "  Product display name: " << this->displayName << ", " << devicePath << std::endl;

    Utf8String componentName;

    //Buttons
    this->state.buttons.resize(buttonCount);
    for (size_t buttonIndex = 0; buttonIndex < this->state.buttons.size(); buttonIndex++)
    {
        componentName = "Button ";
        componentName += Convert::ToString(buttonIndex + 1);
        this->state.buttons[buttonIndex].Reset();
        this->state.buttons[buttonIndex]
            .SetIndex(buttonIndex)
            .SetDisplayName(componentName)
            .SetProcessing(InputButtonProcessing::EVENT_DRIVEN);
    }

    //Axes
    this->state.axes.resize(axisCount);
    for (size_t axisIndex = 0; axisIndex < this->state.axes.size(); axisIndex++)
    {
        componentName = "Axis ";
        componentName += Convert::ToString(axisIndex + 1);
        this->state.axes[axisIndex].Reset();
        this->state.axes[axisIndex]
            .SetIndex(axisIndex)
            .SetMinMax(-32767, 32767)
            .SetDeadZone(8689) //max(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
            .SetProcessing(InputAxisProcessing::NORMALIZE)
            .SetDisplayName(componentName);

        auto& correction = corrections[axisIndex];
        if (correctionCoefCount[correction.type] > 0)
        {
            //std::cout << "axis " << i << ", coef " << correction.coef[0] << ", " << correction.coef[1] << std::endl;
            if (correction.coef[0] == correction.coef[1] && correction.coef[0] != 0)
            {
                //It's a trigger
                //std::cout << "  axis " << i << " is a trigger" << std::endl;
                this->state.axes[axisIndex].SetRestValue(-32767);
            }
        }
    }
}

void LinuxGameController::Destroy()
{
    if (this->fd >= 0)
    {
        close(this->fd);
        this->fd = -1;
    }

    Reset();
}

void LinuxGameController::Update(SimpleTimeDelta elapsedTime, bool isFirstUpdate)
{
    js_event jse;
    int bytesRead = 0;
    do
    {
        bytesRead = read(this->fd, &jse, sizeof(jse));
        if (bytesRead == sizeof(jse))
        {
            switch (jse.type & ~JS_EVENT_INIT)
            {
                case JS_EVENT_AXIS:
                {
                    if (jse.number < this->state.axes.size())
                    {
                        if (jse.type & JS_EVENT_INIT)
                        {
                            //std::cout << "Axis " << (int)jse.number << " value: " << jse.value << std::endl;
                        }
                        else if (!isFirstUpdate)
                            this->state.axes[jse.number].Update(jse.value, isFirstUpdate);
                    }

                    break;
                }
                case JS_EVENT_BUTTON:
                {
                    if (jse.number < this->state.buttons.size())
                    {
                        if (jse.type & JS_EVENT_INIT)
                        {
                            //std::cout << "Button " << (int)jse.number << " value: " << jse.value << std::endl;
                        }
                        else if (!isFirstUpdate)
                            this->state.buttons[jse.number].Update(jse.value, isFirstUpdate);
                    }

                    break;
                }
            }
        }
    }while (bytesRead == sizeof(jse));
}

void LinuxGameController::ClearChanged()
{
    this->state.ClearChanged();

    this->isNewConnection = false;
}

bool LinuxGameController::IsConnected() const
{
    return this->state.isConnected;
}

bool LinuxGameController::GetConnectionChanged() const
{
    return this->state.connectionChanged;
}

bool LinuxGameController::IsNewConnection() const
{
    return this->isNewConnection;
}

size_t LinuxGameController::GetIndex() const
{
    return this->index;
}

size_t LinuxGameController::GetAxisCount() const
{
    return this->state.axes.size();
}

InputAxis* LinuxGameController::GetAxis(size_t axisIndex)
{
    return &this->state.axes[axisIndex];
}

size_t LinuxGameController::GetButtonCount() const
{
    return this->state.buttons.size();
}

InputButton* LinuxGameController::GetButton(size_t buttonIndex)
{
    return &this->state.buttons[buttonIndex];
}

size_t LinuxGameController::GetPovCount() const
{
    return 0;
}

InputPov* LinuxGameController::GetPov(size_t povIndex)
{
    return nullptr;
}

size_t LinuxGameController::GetLocatorCount() const
{
    return 0;
}

InputLocator* LinuxGameController::GetLocator(size_t locatorIndex)
{
    return nullptr;
}

void LinuxGameController::CreateGameControllers(StaticVector<LinuxGameController, GameControllerConstants::MAX_GAME_CONTROLLERS>& gameControllers)
{
    gameControllers.clear();

    Path devicePath;

    for (size_t gameControllerIndex = 0; gameControllerIndex < 50 && !gameControllers.full(); gameControllerIndex++)
    {
        devicePath = "/dev/input/js";
        devicePath += Convert::ToString(gameControllerIndex);

        int fd = open(devicePath.c_str(), O_RDONLY | O_NONBLOCK);
        if (fd >= 0)
        {
            auto gameControllerIndex = gameControllers.size();
            gameControllers.push_back();
            auto& gameController = gameControllers.back();

            gameController.Create(gameControllerIndex, fd, devicePath);
            if (IsReallyGameController(gameController))
                gameController.Update(0, true);
            else
            {
                gameController.Destroy();
                gameControllers.pop_back();
            }
        }
    }
}

void LinuxGameController::UpdateGameControllers
    (
    StaticVector<LinuxGameController, GameControllerConstants::MAX_GAME_CONTROLLERS>& gameControllers,
    SimpleTimeDelta elapsedTime,
    bool isFirstUpdate
    )
{
    for (auto& device : gameControllers)
        device.Update(elapsedTime, isFirstUpdate);
}

bool LinuxGameController::CheckForNewGameControllers(StaticVector<LinuxGameController, GameControllerConstants::MAX_GAME_CONTROLLERS>& gameControllers)
{
    size_t newCount = 0;

    Path devicePath;

    for (size_t gameControllerIndex = 0; gameControllerIndex < 50 && !gameControllers.full(); gameControllerIndex++)
    {
        devicePath = "/dev/input/js";
        devicePath += Convert::ToString(gameControllerIndex);

        if (GetGameControllerByDevicePath(gameControllers, devicePath) == nullptr)
        {
            int fd = open(devicePath.c_str(), O_RDONLY | O_NONBLOCK);
            if (fd >= 0)
            {
                auto gameControllerIndex = gameControllers.size();
                gameControllers.push_back();
                auto& gameController = gameControllers.back();

                gameController.Create(gameControllerIndex, fd, devicePath);
                if (IsReallyGameController(gameController))
                {
                    gameController.isNewConnection = true;
                    gameController.Update(0, true);

                    newCount++;
                }
                else
                {
                    gameController.Destroy();
                    gameControllers.pop_back();
                }
            }
        }
    }

    return newCount > 0;
}
