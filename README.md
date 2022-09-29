# backhoe
Backhoe RC model

JCB 3CX functional model (rough, not copied-model)

https://youtu.be/jnOol_vKNFE

# thingiverse
[FULL](https://www.thingiverse.com/thing:5539326)

parts: (including tinkercad links)
* [AXLE](https://www.thingiverse.com/thing:5538562) x2
* [FRAME](https://www.thingiverse.com/thing:5539294)
* [CABIN](https://www.thingiverse.com/thing:5539225)
* LOADER - loading inwork
* BACKHOE - loading inwork

# materials
* 11x servos like mg996r (i use 3x [25kg*cm](https://aliexpress.ru/item/1005002858520658.html), 1x 996 continuos (i make it from standart 996), 7x [mg996r](https://aliexpress.ru/item/32954832285.html))
* metal servo arm 24.5mm x4 like [this](https://aliexpress.ru/item/32956218385.html)
* metal servo round arm x6 like [this](https://aliexpress.ru/item/32818160492.html)
* arduino uno
* pca9865 pwm16 i2c
* i use 3[18650 plus balances (nominally 12v)
* xl4015 step-down 12v to 5.5v for servos
* power supply 12v-5v-3v3 like [this](https://aliexpress.ru/item/32882313724.html) for arduino & rf24
* rf24 wireless module
* rc remote based on [this](https://www.thingiverse.com/thing:3729116), (not included in this project, you can do it youself) 
* tyres - any 1.9" disk and 40mm width, i use 108x40 with 1.9" (48mm) hole
* a lot of m3 nuts, m3 10/16/20mm screws, a few m3 lock nuts, a few flange m3 nuts
* a lot of 2.5mm screws 8/12mm
* a few m3 30mm (for bucket)

# info
3.2kg weight total, about 2.3kg of abs, ~1.0kg of black and others is yellow (numbers of black abs weight are inaccurate).


# SERVO TORQUE THINKS
Thinking about servo power. My calculations show that the front boom lift requires 8-9kg*cm. With a weight of 250g, you need a power of 16kg, with a weight of 650g, you need a power of 29kg. One 996 lifts an empty bucket. Two 996s don't lift weight in a 250g bucket. One 25kg*cm servo can barely lift 250g of weight. Two 25kg*cm servos lift 650g of weight.
With a drop-down bucket, 45-50 kg * cm will be required (due to two 996 installed on the bucket). Doubled 25kg * cm will not cope.

The load on the servo of the first rear boom is 11-12kg * cm. One 996 does not raise rear boom extended (both rear booms extended). With the second boom half bent, one 996 is difficult to lift. Experiments, when there is weight in the rear bucket, I have not yet carried out, you need to lay plus 5-7kg * cm for the servo of the first arrow.

The load on the servo of the second boom is no more than 8-9 kg * cm, even with a full bucket, one 996 can do it quite well. With a telescopic rear boom, 13kg * cm will be required.

The load on the rotary device exceeds 5 g * cm (failed implementation of the rotary device).

The load on the movement is compensated by protective couplings, 10kg from 996 is enough.


All calculations were performed in ideal "spherical horse in vacuum" conditions, do not take into account losses on the angular levers and friction



# plans
* telescopic second rear boom
* drop-down front bucket
* LED-backlight (headlights and flasher)
