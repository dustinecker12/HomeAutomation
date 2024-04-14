import React, { useState, useEffect } from 'react';
import { StatusBar } from 'expo-status-bar';
import { StyleSheet, View, Pressable } from 'react-native';
import { Slider, Text, Switch } from '@rneui/themed';
import { FontAwesome6 } from '@expo/vector-icons';
import {
  setupAcUnitParamsListener,
  writeAcUnitParams,
} from './helpers/fb-helpers';

export default function App() {
  const [onOffButton, setOnOffButton] = useState(false);
  const [overrideEnabled, setOverrideEnabled] = useState(false);
  const [sliderValue, setSliderValue] = useState(13);
  const [targetTemp, setTargetTemp] = useState(72);
  const [actualTemp, setActualTemp] = useState(72);

  const adjustTargetTemp = () => {
    writeAcUnitParams(
      85 - sliderValue,
      actualTemp,
      onOffButton,
      overrideEnabled
    );
  };

  const toggleOnOffButton = () => {
    writeAcUnitParams(targetTemp, actualTemp, !onOffButton, true);
  };

  const toggleOverrideSwitch = () => {
    writeAcUnitParams(targetTemp, actualTemp, onOffButton, !overrideEnabled);
  };

  useEffect(() => {
    try {
      setupAcUnitParamsListener((state) => {
        setOnOffButton(state.onOffButton);
        setOverrideEnabled(state.overrideEnabled);
        setTargetTemp(state.targetTemp);
        setActualTemp(state.actualTemp);
        setSliderValue(85 - state.targetTemp);
      });
    } catch (error) {
      console.log(error);
    }
  }, []);

  return (
    <View style={styles.container}>
      <StatusBar hidden />
      <View style={styles.infoContainer}>
        <View style={styles.infoCard}>
          <Text style={styles.infoText}>
            <FontAwesome6
              name="temperature-arrow-up"
              size={24}
              color="#2EC487"
            />{' '}
            Set
          </Text>
          <Text style={styles.infoText}>
            {!overrideEnabled ? `${targetTemp}°` : 'Overridden'}
          </Text>
        </View>
        <View style={styles.infoCard}>
          <Text style={styles.infoText}>
            <FontAwesome6
              name="temperature-arrow-down"
              size={24}
              color="#F24361"
            />{' '}
            Actual
          </Text>
          <Text style={styles.infoText}>{actualTemp}°</Text>
        </View>
      </View>
      <View style={styles.controlsContainer}>
        <Slider
          style={styles.slider}
          value={sliderValue}
          maximumTrackTintColor="#48cae4"
          maximumValue={25}
          minimumTrackTintColor="#555555"
          minimumValue={0}
          onSlidingComplete={() => adjustTargetTemp()}
          onValueChange={(sliderValue) => {
            setSliderValue(sliderValue);
          }}
          orientation="vertical"
          step={1}
          allowTouchTrack
          thumbStyle={{
            height: 50,
            width: 50,
            borderRadius: 25,
            backgroundColor: '#48cae4',
          }}
          thumbProps={{
            children: (
              <View
                style={{
                  flex: 1,
                  justifyContent: 'center',
                }}
              >
                <Text
                  style={{
                    color: '#1F2124',
                    textAlign: 'center',
                    fontWeight: 'bold',
                    fontSize: 18,
                  }}
                >
                  {85 - sliderValue}
                </Text>
              </View>
            ),
          }}
          trackStyle={{
            width: 10,
            borderRadius: 10,
          }}
        />
        <View style={{ borderRadius: 50, overflow: 'hidden' }}>
          <Pressable
            style={onOffButton ? styles.onOffButtonOn : styles.onOffButtonOff}
            android_ripple={{ color: '#3a506b', borderless: false }}
            onPress={() => {
              toggleOnOffButton();
            }}
          >
            {onOffButton ? (
              <Text style={styles.onOffButtonTextOn}>On</Text>
            ) : (
              <Text style={styles.onOffButtonTextOff}>Off</Text>
            )}
          </Pressable>
        </View>
        <View>
          <Switch
            style={styles.overrideSwitch}
            thumbColor="#48cae4"
            trackColor={{ true: '#48cae4', false: '#555555' }}
            value={overrideEnabled}
            onValueChange={() => toggleOverrideSwitch()}
          />
          <Text
            style={{ color: '#EDF1F3' }}
            onPress={() => toggleOverrideSwitch()}
          >
            Override
          </Text>
        </View>
      </View>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    flexDirection: 'column',
    backgroundColor: '#1F2124',
    alignItems: 'center',
    justifyContent: 'center',
  },
  infoContainer: {
    flex: 1,
    flexDirection: 'row',
    alignItems: 'center',
    justifyContent: 'center',
    width: '100%',
  },
  infoCard: {
    flex: 1,
    flexDirection: 'column',
    alignItems: 'center',
    justifyContent: 'center',
  },
  infoText: {
    color: '#EDF1F3',
    fontSize: 24,
  },
  controlsContainer: {
    flex: 3,
    flexDirection: 'column',
    alignItems: 'center',
    justifyContent: 'space-around',
    width: '100%',
  },
  slider: {
    height: 250,
    margin: 25,
  },
  onOffButtonOff: {
    justifyContent: 'center',
    backgroundColor: 'transparent',
    borderColor: '#48cae4',
    borderWidth: 1,
    borderRadius: 50,
    width: 150,
    height: 50,
  },
  onOffButtonOn: {
    justifyContent: 'center',
    backgroundColor: '#48cae4',
    borderColor: '#48cae4',
    borderWidth: 1,
    borderRadius: 50,
    width: 150,
    height: 50,
  },
  onOffButtonTextOff: {
    textAlign: 'center',
    color: '#48cae4',
    fontWeight: 'bold',
    fontSize: 20,
  },
  onOffButtonTextOn: {
    textAlign: 'center',
    color: '#1F2124',
    fontWeight: 'bold',
    fontSize: 20,
  },
});
