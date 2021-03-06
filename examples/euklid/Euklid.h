/*
        ##########    Copyright (C) 2015 Vincenzo Pacella
        ##      ##    Distributed under MIT license, see file LICENSE
        ##      ##    or <http://opensource.org/licenses/MIT>
        ##      ##
##########      ############################################################# shaduzlabs.com #####*/

#pragma once

#include <future>
#include <stdint.h>

#include <cabl.h>

#include "Sequence.h"

#define __MACOSX_CORE__ 1
#include "RtMidi.h"

namespace sl
{

using namespace cabl;

class Euklid
{
public:

  enum class EncoderState
  {
    Length,
    Pulses,
    Rotate,
    Shuffle,
    Speed,
  };

  enum class ScreenPage
  {
    Sequencer,
    Configuration,
  };

  Euklid();
  void run();
  
  void initHardware();
  void tick();
  void discoverAndConnect();
  
  void buttonChanged(Device::Button button_, bool buttonState_, bool shiftState_);
  void encoderChanged(Device::Encoder encoder_, bool valueIncreased_, bool shiftPressed_);
  void padChanged(Device::Pad pad_, uint16_t value_, bool shiftPressed);
  void keyChanged(Device::Key key_, uint16_t value_, bool shiftPressed);
  
  
  void updateClock();
  
  void play();

  void updateGUI();
  void updateGroupLeds();
  void updatePads();
  
  void drawConfigurationPage();
  void drawSequencerPage();

  void setEncoderState(EncoderState encoderState_)
  {
    m_encoderState = encoderState_;
  }

  ScreenPage getScreenPage() const{ return m_screenPage; }
  void setScreenPage(ScreenPage screenPage_)
  {
    m_screenPage = screenPage_;
  }

  void setEncoder(bool valueIncreased_, bool shiftPressed_);

  void togglePlay();

  void changeTrack(uint8_t track_ = 0xFF);
  void nextTrack();
  void prevTrack();
  
private:

  uint8_t getEncoderValue(
    bool valueIncreased_,
    uint8_t step_,
    uint8_t currentValue_,
    uint8_t minValue_,
    uint8_t maxValue_
  );
  Device::Pad getPad(uint8_t padIndex_);
  uint8_t getPadIndex(Device::Pad pad_);

  Client       m_client;
  
  Sequence<uint16_t> m_sequences[3];

  std::future<void> m_clockFuture;
  EncoderState m_encoderState;
  ScreenPage m_screenPage;

  uint8_t m_lengths[3];
  uint8_t m_pulses[3];
  uint8_t m_rotates[3];

  std::atomic<bool> m_play;
  uint8_t m_currentTrack;

  uint8_t m_bpm;
  uint8_t m_shuffle;

  RtMidiOut* m_pMidiout;

  uint8_t   m_quarterNote;
  uint16_t  m_delayEven;
  uint16_t  m_delayOdd;
  
  std::atomic<bool> m_update;
  
};

} // namespace sl
