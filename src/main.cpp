#include <iostream>
#include <cmath>

struct RGB {
    double red;
    double green;
    double blue;
};

const RGB GOOD  = {0x00, 0x64, 0x00}; // 0-50, green
//https://www.colorhexa.com/00e400-to-ffff00
const RGB MODERATE  = {0xFF, 0xFF, 0x00}; //50-100, yellow
//https://www.colorhexa.com/ffff00-to-ff7e00
const RGB UNHEALTHY  = {0xE8, 0x74, 0x00}; //100-150, orange
//https://www.colorhexa.com/ff7e00-to-ff0000
const RGB VERY_UNHEALTHY  = {0xFF, 0x00, 0x00};//150-200, red
//https://www.colorhexa.com/ff0000-to-9400d3
const RGB HAZARDOUS  = {0x94, 0x00, 0xD3};//200-300, purple
//https://www.colorhexa.com/9400d3-to-800000
const RGB DANGEROUS  = {0x8B, 0x45, 0x13}; //300+, brown



double from_sRGB(double x) {
    //Returns a linear value in the range [0,1]    for sRGB input in [0,255].
    x /= 255.0;
    if (x <= 0.04045)
        return x / 12.92;
    else
        return pow((x + 0.055) / 1.055, 2.4);
}

RGB from_sRGB(RGB color) {
    return {from_sRGB(color.red), from_sRGB(color.green), from_sRGB(color.blue)};
}

double to_sRGB_f(double x) {
    // Returns a sRGB value in the range [0,1] for linear input in [0,1].
    if (x <= 0.0031308)
        return 12.92 * x;
    else
        return (1.055 * (pow(x, 1 / 2.4))) - 0.055;
}

int to_sRGB(double x) {
    // Returns a sRGB value in the range [0,255] for linear input in [0,1]
    return int(255.9999 * to_sRGB_f(x));
}


RGB to_sRGB(RGB color) {
    // Returns a sRGB value in the range [0,255] for linear input in [0,1]
    RGB result = {
            .red = (double)to_sRGB(color.red),
            .green= (double)to_sRGB(color.green),
            .blue= (double)to_sRGB(color.blue)
    };
    return result;
}


double lerp(double a, double b, double x) {
    return a + x * (b - a);
}


RGB lerp(RGB a, RGB b, double x) {
    return {
            a.red + x * (b.red - a.red),
            a.green + x * (b.green - a.green),
            a.blue + x * (b.blue - a.blue)
    };
}

double sum(RGB color) {
    return color.red + color.green + color.blue;
}

void perceptual_steps(RGB color1, RGB color2, struct RGB gradient[], int maxSteps) {
    double gamma = .43;


    RGB color1_lin = from_sRGB(color1);
    RGB color2_lin = from_sRGB(color2);


    double bright1 = pow(sum(color1_lin), gamma);
    double bright2 = pow(sum(color2_lin), gamma);

    for (int i = 0; i < maxSteps; i++) {
        double intensity = pow(lerp(bright1, bright2, (double) i / (maxSteps - 1)), 1 / gamma);
        RGB color = lerp(color1_lin, color2_lin, (double) i / (maxSteps - 1));
        if (sum(color) != 0) {
            color.red = color.red * intensity / sum(color);
            color.green = color.green * intensity / sum(color);
            color.blue = color.blue * intensity / sum(color);
        }
        color = to_sRGB(color);

        gradient[i] = color;
    }
}

RGB getPerceptualStep(RGB color1, RGB color2, int step, int maxSteps) {
    struct RGB steps[maxSteps];
    perceptual_steps(color1, color2, steps, maxSteps);
    return steps[step];
}

RGB getColor(int aqi) {
    if (aqi <= 30) return GOOD;
    if (aqi < 50) return getPerceptualStep(GOOD, MODERATE, aqi - 30, 20);
    if (aqi <= 80) return MODERATE;
    if (aqi < 100) return getPerceptualStep(MODERATE, UNHEALTHY, aqi - 80, 20);
    if (aqi <= 130) return UNHEALTHY;
    if (aqi < 150) return getPerceptualStep(UNHEALTHY, VERY_UNHEALTHY, aqi - 130, 20);
    if (aqi <= 180) return VERY_UNHEALTHY;
    if (aqi < 200) return getPerceptualStep(VERY_UNHEALTHY, HAZARDOUS, aqi - 180, 20);
    if (aqi <= 260) return HAZARDOUS;
    if (aqi < 300) return getPerceptualStep(HAZARDOUS, DANGEROUS, aqi - 260, 40);
    return DANGEROUS;
}

int main() {

    std::cout << "static const uint8_t PROGMEM aiq_colors[] =  {" << std::endl;

    for(int i=1; i<=500; i++){
        RGB color = getColor(i);
        printf("0x%.2X, 0x%.2X, 0x%.2X,\n", (int) color.red,(int) color.green,(int) color.blue);
    }
    std::cout << "};" << std::endl;
    return 0;
}
