# Color gradient algo for AIQ sensor

Adjust colors, steps and boundaries, run it to generate a static map, store it in `PROGMEM`

[Color Gradient](https://stackoverflow.com/questions/22607043/color-gradient-algorithm) by [Mark Ransom](https://stackoverflow.com/users/5987/mark-ransom)

"...The intensity of the gradient must be constant in a perceptual color space or it will look unnaturally dark or light at points in the gradient. You can see this easily in a gradient based on simple interpolation of the sRGB values, particularly the red-green gradient is too dark in the middle. Using interpolation on linear values rather than gamma-corrected values makes the red-green gradient better, but at the expense of the back-white gradient. By separating the light intensities from the color you can get the best of both worlds.

Often when a perceptual color space is required, the Lab color space will be proposed. I think sometimes it goes too far, because it tries to accommodate the perception that blue is darker than an equivalent intensity of other colors such as yellow. This is true, but we are used to seeing this effect in our natural environment and in a gradient you end up with an overcompensation.

A power-law function of 0.43 was experimentally determined by researchers to be the best fit for relating gray light intensity to perceived brightness..."

```
Algorithm MarkMix
   Input:
      color1: Color, (rgb)   The first color to mix
      color2: Color, (rgb)   The second color to mix
      mix:    Number, (0..1) The mix ratio. 0 ==> pure Color1, 1 ==> pure Color2
   Output:
      color:  Color, (rgb)   The mixed color

   //Convert each color component from 0..255 to 0..1
   r1, g1, b1 ← Normalize(color1)
   r2, g2, b2 ← Normalize(color1)

   //Apply inverse sRGB companding to convert each channel into linear light
   r1, g1, b1 ← sRGBInverseCompanding(r1, g1, b1)       
   r2, g2, b2 ← sRGBInverseCompanding(r2, g2, b2)

   //Linearly interpolate r, g, b values using mix (0..1)
   r ← LinearInterpolation(r1, r2, mix)
   g ← LinearInterpolation(g1, g2, mix)
   b ← LinearInterpolation(b1, b2, mix)

   //Compute a measure of brightness of the two colors using empirically determined gamma
   gamma ← 0.43
   brightness1 ← Pow(r1+g1+b1, gamma)
   brightness2 ← Pow(r2+g2+b2, gamma)

   //Interpolate a new brightness value, and convert back to linear light
   brightness ← LinearInterpolation(brightness1, brightness2, mix)
   intensity ← Pow(brightness, 1/gamma)

   //Apply adjustment factor to each rgb value based
   if ((r+g+b) != 0) then
      factor ← (intensity / (r+g+b))
      r ← r * factor
      g ← g * factor
      b ← b * factor
   end if

   //Apply sRGB companding to convert from linear to perceptual light
   r, g, b ← sRGBCompanding(r, g, b)

   //Convert color components from 0..1 to 0..255
   Result ← MakeColor(r, g, b)
End Algorithm MarkMix
```