// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot;

import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.Joystick;

/** This is a demo program showing the use of GenericHID's rumble feature. */
public class Robot extends TimedRobot {
  private final Joystick m_hid = new Joystick(0);
  private int m_output = 40000;

  @Override
  public void autonomousInit() {
    m_hid.setOutputs(0);
  }

  @Override
  public void robotPeriodic() {
      m_hid.setOutputs(m_output);
      m_output = (m_output + 1) % 0xffff;
  }

  @Override
  public void disabledInit() {
    m_hid.setOutputs(0);
  }
}
