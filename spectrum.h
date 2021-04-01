#ifndef SPECTRUM_H_
#define SPECTRUM_H_

#define PI 3.1415926535f
#define BUFFER_SIZE 16384

#include <SFML\Graphics.hpp>
#include <SFML/Audio.hpp>
#include <complex>
#include <iostream>

class Spectrum : protected Drawable
{
private:

    typedef std::complex<float> Complex;
    typedef std::vector<Complex> Verticies;

    float *_hammingWindow;

    sf::VertexArray _VA1, _VA2;
    Verticies _samples;
    sf::SoundBuffer _buffer;
    sf::Music _audio;

    void FFT(Verticies&);
    void calculateSamples();
    void updateSpectrum();

public:
    Spectrum();
    ~Spectrum();

    void setSong(const std::string&);
    void play(bool);
    void stop();
    void draw(sf::RenderWindow&);
};

#endif // SPECTRUM_H_
