/*
        ##########    Copyright (C) 2015 Vincenzo Pacella
        ##      ##    Distributed under MIT license, see file LICENSE
        ##      ##    or <http://opensource.org/licenses/MIT>
        ##      ##
##########      ############################################################# shaduzlabs.com #####*/

#include "client/Client.h"

#include <algorithm>
#include <thread> //remove and use a custom sleep function!
#include <iostream>

#include "cabl.h"

#include "devices/ableton/Push2.h"
#include "devices/ableton/Push2Display.h"
#include "devices/akai/Push.h"
#include "devices/ni/KompleteKontrol.h"
#include "devices/ni/MaschineMK1.h"
#include "devices/ni/MaschineMK2.h"
#include "devices/ni/MaschineMikroMK2.h"
#include "devices/ni/TraktorF1MK2.h"

namespace
{
static const unsigned kClientMaxConsecutiveErrors = 100;
}

namespace sl
{
namespace cabl
{

//--------------------------------------------------------------------------------------------------

Client::tCollDrivers Client::s_collDrivers = Client::tCollDrivers();

//--------------------------------------------------------------------------------------------------

Client::Client()
{
  M_LOG("Controller Abstraction Library v. " << Lib::getVersion());
}

//--------------------------------------------------------------------------------------------------

Client::~Client()
{
  M_LOG("[Client] destructor");
  if (m_cablThread.joinable())
  {
    m_cablThread.join();
  }
}

//--------------------------------------------------------------------------------------------------

void Client::run()
{
  m_clientStopped = false;
  m_connected = false;
  m_cablThread = std::thread(
    [this]()
    {
      while (!m_clientStopped)
      {

        //\todo remove enumerateDevices call!
        /*
        m_connected = false;
        auto collDevices = enumerateDevices();
        if (collDevices.size() > 0) // found known devices
        {
          connect(collDevices[0]);
        }
*/
        if(m_connected)
        {
          m_pDevice->init();
          onConnected();
          M_LOG("[Application] run: device connected" );
          unsigned nErrors = 0;
          while (m_connected)
          {
            onTick();
            if(!m_pDevice->tick())
            {
              nErrors++;
              if (nErrors >= kClientMaxConsecutiveErrors)
              {
                m_connected = false;
                M_LOG("[Application] run: disconnected after " << nErrors << " errors" );
              }
            }
            else
            {
              nErrors = 0;
            }
          }
          onDisconnected();
        }
        else
        {
          std::this_thread::yield();
        }
      }
    }
  );
}

//--------------------------------------------------------------------------------------------------

void Client::stop()
{
  M_LOG("[Client] stop");
  m_connected = false;
  m_clientStopped = true;
}

//--------------------------------------------------------------------------------------------------

Driver::tCollDeviceDescriptor Client::enumerateDevices()
{
  Driver::tCollDeviceDescriptor devicesList;
#if defined(_WIN32) || defined(__APPLE__) || defined(__linux)
  for (const auto& deviceDescriptor : getDriver(Driver::Type::HIDAPI)->enumerate())
  {
    if (!DeviceFactory::instance().isKnownDevice(deviceDescriptor))
    {
      continue; // not a Native Instruments USB device
    }
    devicesList.push_back(deviceDescriptor);
  }
  M_LOG(
    "[Application] enumerateDevices: " << devicesList.size() << " known devices found via HIDAPI");

  unsigned nFoundMidi = 0;
  for (const auto& deviceDescriptor : getDriver(Driver::Type::MIDI)->enumerate())
  {
    if (!DeviceFactory::instance().isKnownDevice(deviceDescriptor))
    {
      continue; // not a Native Instruments USB device
    }
    nFoundMidi++;
    devicesList.push_back(deviceDescriptor);
  }
  M_LOG("[Application] enumerateDevices: " << nFoundMidi << " known devices found via MIDI");

  Driver::Type tMainDriver(Driver::Type::LibUSB);
#endif

  for (const auto& deviceDescriptor : getDriver(tMainDriver)->enumerate())
  {
    if ((!DeviceFactory::instance().isKnownDevice(deviceDescriptor))
        || (std::find(devicesList.begin(), devicesList.end(), deviceDescriptor)
             != devicesList.end()))
    {
      continue; // unknown
    }
    devicesList.push_back(deviceDescriptor);
  }
  M_LOG("[Application] enumerateDevices: " << devicesList.size() << " total known devices found");

  return devicesList;
}

//--------------------------------------------------------------------------------------------------

bool Client::connect(const DeviceDescriptor& deviceDescriptor_)
{
  m_connected = false;
  if (!deviceDescriptor_)
  {
    return false;
  }

#if defined(_WIN32) || defined(__APPLE__) || defined(__linux)
  Driver::Type driverType;
  switch (deviceDescriptor_.getType())
  {
    case DeviceDescriptor::Type::HID:
    {
      driverType = Driver::Type::HIDAPI;
      break;
    }
    case DeviceDescriptor::Type::MIDI:
    {
      driverType = Driver::Type::MIDI;
      break;
    }
    case DeviceDescriptor::Type::USB:
    default:
    {
      driverType = Driver::Type::LibUSB;
      break;
    }
  }
#endif
  auto deviceHandle = getDriver(driverType)->connect(deviceDescriptor_);

  if (deviceHandle)
  {
    m_pDevice = DeviceFactory::instance().getDevice(deviceDescriptor_, std::move(deviceHandle));
    m_connected = (m_pDevice != nullptr);
  }

  return m_connected;
}
//--------------------------------------------------------------------------------------------------

void Client::setLed(Device::Button btn_, const util::LedColor& color_)
{
  if(m_pDevice)
  {
    m_pDevice->setLed(btn_, color_);
  }
}

//--------------------------------------------------------------------------------------------------

void Client::setLed(Device::Pad pad_, const util::LedColor& color_)
{
  if(m_pDevice)
  {
    m_pDevice->setLed(pad_, color_);
  }
}

//--------------------------------------------------------------------------------------------------

void Client::setLed(Device::Key key_, const util::LedColor& color_)
{
  if(m_pDevice)
  {
    m_pDevice->setLed(key_, color_);
  }
}

//--------------------------------------------------------------------------------------------------

void Client::onConnected()
{
  if(m_cbConnected)
  {
    m_cbConnected();
  }
}

//--------------------------------------------------------------------------------------------------

void Client::onTick()
{
  if(m_cbTick)
  {
    m_cbTick();
  }
}

//--------------------------------------------------------------------------------------------------

void Client::onDisconnected()
{
  if(m_cbDisconnected)
  {
    m_cbDisconnected();
  }
}

//--------------------------------------------------------------------------------------------------

Client::tDriverPtr Client::getDriver(Driver::Type tDriver_)
{
  if (s_collDrivers.find(tDriver_) == s_collDrivers.end())
  {
    s_collDrivers.emplace(tDriver_, std::make_shared<Driver>(tDriver_));
  }

  return s_collDrivers[tDriver_];
}

//--------------------------------------------------------------------------------------------------

} // cabl
} // sl
