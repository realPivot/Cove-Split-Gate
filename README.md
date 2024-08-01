# Split Gate by Cove
 A dual-band downward expansion plugin designed to help with the gating of specific frequency bands. 
 The bands are separated by Linkwitz Riley filters, which are typically used for crossovers. Due to 
 this, there is a small phase shift at the crossover point, but the upside is that it is quick enough 
 to use in real time. If you want to use this on a multi-mic'd source (AKA a snare drum in a drumset), 
 be aware of that phase shift.

 The metering is not 100% accurate (as of 0.95) but it does get the job done to give you some contextual
 information about what you're gating. Each band can have their gate enabled/disabled and also each band
 can be muted individually (giving a faux solo).

 ## There are 5 basic parameters for each gate:

 ### Threshold
 What level the gate should start decreasing volume at

 ### Ratio
 How much the gate should decrease the level (higher ratio = more volume loss)

 ### Attack
 How quickly the gate should open up after being closed

 ### Release
 How quickly the gate should close after volume drops below the threshold

 ### Hold
 How long the gate should hold open before reacting to dropping below the threshold


 All of these parameters have been given generously low amounts, which can cause distortion. Be aware when setting attack/release/hold too low.

 
