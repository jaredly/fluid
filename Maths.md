
GIVEN:
- damping_ratio between 0 and 1
- frequency_response in ms
- x = current position
- v = current velocity

DERIVE:
- accelleration

assuming a mass of 1 I think

####

damping_ratio = b / (2*sqrt(k))

** b in terms of k **
b = damping_ratio * 2 * sqrt(k)

accelleration = -k * x + -b * v

#####

b = damping_factor * 2 * sqrt(k)

accelleration = -k * x + -(damping_ratio * 2 * sqrt(k)) * v

w = sqrt(k * (1 - 2 * damping_ratio))

w^2 = k * (1 - 2 * damping_ratio)

k = frequency_response ^ 2 / (1 - 2 * damping_ratio)

accelleration = -k * x + -(damping_ratio * 2 * sqrt(k)) * v





What's frequency repsonse?

For underdamped, it's:

w^2 = - (b^2 - 2k)/(2)

w = sqrt(k - b^2/(2))

in terms of damping ratio:


damping_factor = sqrt(2)/2 * damping_ratio

w = undamped_w * sqrt(1 - 2 * damping_factor * damping_factor)

undamped_w = sqrt(k)











// equations with mass

zeta = b / (2*sqrt(k * m))

1 = b / (2*sqrt(k * m))

** b in terms of k & m **
b = damping_ratio * 2 * sqrt(k * m)


accelleration = -k * x + -b * v




What's frequency repsonse?

For underdamped, it's:

w^2 = - (b^2 - 2km)/(2m^2)

w = sqrt(k/m - b^2/(2m^2))

in terms of damping ratio:

w = undamped_w * sqrt(1 - 2 * damping_ratio)

undamped_w = sqrt(k / m)





#### The math, again

http://mathworld.wolfram.com/SimpleHarmonicMotion.html

accel = -w^2 * x

where (w^2) is I think (k = "stiffness") in our normal spring equations.

http://mathworld.wolfram.com/DampedSimpleHarmonicMotion.html
with damping

accel = -damping * v - w^2 * x


Ok "Damping regime"

D = damping^2 - 4 * w^2

if D = 0, it's critically damped.
if D < 0, it's underdamped (will oscillate)
if D > 0, it's overdamped (will take longer to get to rest)











