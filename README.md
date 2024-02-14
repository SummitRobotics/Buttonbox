# Buttonbox
## installation
* From Arduino IDE, go to Sketch / Include Library / Add ZIP Library... and install all the .zip files from `libraries/` directory.

** All the libraries, except `JoystickWithDSOutputs.zip`, can also be installed through the Arduino Library Manager. Following are the versions provided in the .zip files:

- Adafruit BusIO - 1.15.0
- Adafruit GFX Library - 1.11.9
- Adafruit SH110X - 2.1.10
- Encoder - 1.4.4
- FastLED - 3.6.0 (used for light strips and not buttonbox)
- JoystickWithDSOutputs - packaged from https://github.com/ohowe1/LeonardoWithDSOutputs

## Buttonbox Message Type

The Buttonbox code (see /FinalDesign/FinalDesign) consumes a 16-bit message from the driver station.


* This message is interpreted as:

| Field | Bit Range | Description |
| ---- | ---- | ---- |
| Message Type | 14-15 | 0 - LedsUpdate<br>1 - ViewUpdate |
| Message Dependent | 0 - 13 | xxxx |
|  |  |  |
| **LedsUpdate** | 14-15 | 0 |
| Unused | 9-13 | xxxx |
| LedEnabled | 0-8 | 1- turn on associated led, 0 off |
|  |  |  |
| **ViewUpdate** | 14-15 | 1 |
| VeiwType | 12-13 | 0 - None<br>1 - Progress (rectangle)<br>2 - Orientaion (triangle)<br>3 - Counter (numbers) |
| Frame | 11 | 1 - turn on frame/border (MO?), 0 - off |
| Value | 0-10 | [0-100] for progress<br>[0, 360) for orientation/angle<br># for counter  |

* Example robot code to send updates to joystick:
```java
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
      // Run through all LED combinations...
      m_hid.setOutputs(m_output);
      m_output = (m_output + 1) % 0x1ff;
  }

  @Override
  public void disabledInit() {
    m_hid.setOutputs(0);
  }
}
```