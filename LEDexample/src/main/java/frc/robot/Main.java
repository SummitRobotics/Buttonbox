// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot;

import edu.wpi.first.wpilibj.RobotBase;
import edu.wpi.first.wpilibj.AddressableLED;
import edu.wpi.first.wpilibj.AddressableLEDBuffer;
/**
 * Do NOT add any static variables to this class, or any initialization at all. Unless you know what
 * you are doing, do not modify this file except to change the parameter class to the startRobot
 * call.
 */
public final class Main {
  
  private Main() {
   
  }

  /**
   * Main initialization function. Do not perform any initialization here.
   *
   * <p>If you change your main robot class, change the parameter type.
   */
  public static void main(String... args) {
    int LED_PORT = 0;
    int LED_LENGTH = 12;

    AddressableLED ledStrip = new AddressableLED(LED_PORT);
    AddressableLEDBuffer buffer = new AddressableLEDBuffer(LED_LENGTH);
     for (int i = 0;i<LED_LENGTH;i++){
      buffer.setRGB(i, 255,0,0);
    }
        ledStrip.setLength(LED_LENGTH);
        ledStrip.setData(buffer);
        ledStrip.start();
        System.out.println("hello world");
    RobotBase.startRobot(Robot::new);
  
  }
}
