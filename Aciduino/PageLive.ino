/*
[midi controller]
knobs: cutoff freq./decay, resonance/accent, env mod/tunning, tempo

buttons: track 1, track 2, ctrl A/ctrl B, tempo -, tempo +, play/stop
*/
// TODO: implement pickup by value for controllers

uint8_t _selected_ctrl = 0;

void processControllerButtons()
{
  // select track 1
  if ( released(GENERIC_BUTTON_1) ) {
    lockPotsState(true);
    _selected_track = 0;
  }

  // select track 2
  if ( released(GENERIC_BUTTON_2) ) {
    lockPotsState(true);
    _selected_track = 1;
  }

  // toogle between ctrl A and ctrl B setup for potentiometers
  if ( pressed(GENERIC_BUTTON_3) ) {
    lockPotsState(true);
    _selected_ctrl = !_selected_ctrl;
  }

  // decrement 1 bpm from tempo
  if ( pressed(GENERIC_BUTTON_4) ) {
    uClock.setTempo(uClock.getTempo()-1);
  }

  // increment 1 bpm from tempo
  if ( pressed(GENERIC_BUTTON_5) ) {
    uClock.setTempo(uClock.getTempo()+1);
  }

}

void processControllerLeds()
{

  digitalWrite(GENERIC_LED_4, LOW);
  digitalWrite(GENERIC_LED_5, LOW); 
    
  if ( _selected_track == 0 ) {
    digitalWrite(GENERIC_LED_1, HIGH);
    digitalWrite(GENERIC_LED_2, LOW);
  } else if ( _selected_track == 1 ) {
    digitalWrite(GENERIC_LED_1, LOW);
    digitalWrite(GENERIC_LED_2, HIGH);
  } 
  
  if ( _selected_ctrl == 0 ) {
    digitalWrite(GENERIC_LED_3, LOW);
  } else if ( _selected_ctrl == 1 ) {
    digitalWrite(GENERIC_LED_3, HIGH);
  } 

}

void processControllerPots()
{
  uint16_t value;
  uint8_t ctrl;
  
  // GENERIC_POT_1: cutoff freq./decay
  value = getPotChanges(GENERIC_POT_1, 0, 127);
  if ( value != -1 ) {  
    // send cc
    if ( _selected_ctrl == 0 ) {
      ctrl = MIDI_CTRL_CUTOFF;
    } else if ( _selected_ctrl == 1 ) {
      ctrl = MIDI_CTRL_DECAY;
    }
    sendMidiMessage(MIDI_CC, ctrl, value, _sequencer[_selected_track].channel, true);    
  }  

  // GENERIC_POT_2: resonance/accent
  value = getPotChanges(GENERIC_POT_2, 0, 127);
  if ( value != -1 ) {  
    // send cc
    if ( _selected_ctrl == 0 ) {
      ctrl = MIDI_CTRL_RESONANCE;
    } else if ( _selected_ctrl == 1 ) {
      ctrl = MIDI_CTRL_ACCENT;
    }
    sendMidiMessage(MIDI_CC, ctrl, value, _sequencer[_selected_track].channel, true);      
  }  

  // GENERIC_POT_3: env mod/wave
  value = getPotChanges(GENERIC_POT_3, 0, 127);
  if ( value != -1 ) {  
    // send cc
    if ( _selected_ctrl == 0 ) {
      ctrl = MIDI_CTRL_ENVMOD;
    } else if ( _selected_ctrl == 1 ) {
      ctrl = MIDI_CTRL_WAVE;
    }
    sendMidiMessage(MIDI_CC, ctrl, value, _sequencer[_selected_track].channel, true);     
  }    

  // GENERIC_POT_4: sequencer step length/global harmonic mode transpose 
  if ( _selected_ctrl == 0 ) {
    value = getPotChanges(GENERIC_POT_4, 1, STEP_MAX_SIZE);
    if ( value != -1 ) {  
      clearStackNote(_selected_track);
      ATOMIC(_sequencer[_selected_track].step_length = value);
      if ( _step_edit >= _sequencer[_selected_track].step_length ) {
        _step_edit = _sequencer[_selected_track].step_length-1;
      }      
    }
  } else if ( _selected_ctrl == 1 ) {
    value = getPotChanges(GENERIC_POT_4, 0, 24);
    if ( value != -1 ) {  
      clearStackNote();
      // -12 (0) +12 
      ATOMIC(_transpose = value-12); 
    }  
  }
   
}
