# Paint Perfector Prototype

The Paint Perfector solves the customer problem of obtaining small amounts of highly specific paint colors
quickly and accurately without the inconvenience of traditional paint-matching methods. Customers often need exact
shades for touch-ups, repairs, or small projects, but finding these colors in stores can be time-consuming and wasteful,
especially when larger paint quantities are all that is offered. The Paint Perfector fulfills this unmet need by providing
users with precise, custom-matched paint in small, on-demand quantities. This unique capability saves customers
time, reduces material waste, and offers a level of convenience and personalization that may be unavailable.  

The Paint Perfector is an innovative solution allowing the user to scan whatever color they want to replicate
and quickly recreating it with high accuracy, skipping the frustration of traditional color-matching
methods. Targeted toward homeowners, DIY enthusiasts, and professionals needing precise touch-ups, the product
eliminates the time-consuming process of searching for exact shades in stores. By generating small amounts of perfectly matched paint on demand, the Paint Perfector reduces excess waste and lowers costs for users who only need
limited quantities. Its primary value proposition lies in its speed, precision, and convenience, combining advanced
color-matching capabilities with efficient paint mixing to provide users with immediate, customized results.  

The device uses the DFRobot TCS34825 RGB Color Sensor utilizing I2C protocol; it synchronizes
over the SCL line and sends data over the SDA line. The data from the color sensor is converted from RGB to hexadecimal
and displayed on the U.I so the user can check the sensor’s accuracy before mixing. Once the mix
button is pressed, RGB data is converted to CMYK and sent to the slave Arudino to be converted into steps.  
The stepper motors will read this data and each syringe will dispense a calculated amount of paint to replicate the  
scanned color. Finally, the user places the mixer over the cup with the paint and turns it on until the paint is fully mixed.  

Our prototype introduces a novel approach to precise paint matching by integrating a DFRobot
TCS34825 RGB color sensor using I2C communication, allowing real-time color detection. Its innovation lies in
converting raw RGB values and then displaying them as a HEX code on the user interface,
giving users visual confirmation of accuracy. Once confirmed, the system automatically calculates and dispenses
exact paint quantities through syringe-based actuators. Future improvements could include enhanced sensor  
calibration for more complex lighting.  

Development Team: J. Crain, N. Gattis, G. Fast, N. Lindsey  
Project Coordinator: Oshiv Yuvaraj  
Department of Electrical and Computer Engineering, Mississippi State University  
413 Hardy Road, Box 9571  
Mississippi State, Mississippi 39762  
Email: {jgc345, nwg50, glf79, nbl76}@msstate.edu
