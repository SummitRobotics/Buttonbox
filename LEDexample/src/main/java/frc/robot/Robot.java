// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot;

import edu.wpi.first.wpilibj.TimedRobot;

import frc.robot.devices.LEDs.LEDs;
import frc.robot.devices.LEDs.LEDCalls;

/** This is a demo program showing the use of GenericHID's rumble feature. */
public class Robot extends TimedRobot {
    // int LED_PORT = 0;
    // int LED_LENGTH = 12;
    // AddressableLED ledStrip = new AddressableLED(LED_PORT);
    // AddressableLEDBuffer buffer = new AddressableLEDBuffer(LED_LENGTH);

  // NOTE: need amber / blue for tomorrow 
  @Override
  public void robotInit() {
    // LEDCalls.ON.activate();
    // LEDCalls.INTAKE_DOWN.activate();
    LEDCalls.AMPLIFY.activate();;
    // LEDCalls.MO.activate();
    // for (int i = 0; i < LED_LENGTH; i++) {
    //   buffer.setRGB(i, 0, 255, 0);
    // }
    // ledStrip.setLength(LED_LENGTH);
    // ledStrip.setData(buffer);
    // ledStrip.start();
  }

  @Override
  public void robotPeriodic() {
    LEDs.getInstance().periodic();
  }

  @Override
  public void autonomousInit() {
  }

  @Override
  public void disabledInit() {
  }
}
