#include "../../spectrum.h"

/**
 Fast-Fourior-Transmition Algorithm 
 @param- Vector Base Reference
 */
void Spectrum::FFT(Verticies &vB)
{
	for (size_t i = 1, j = 0; i < vB.size(); ++i)
	{
		int bit = vB.size() >> 1;
		for (; j >= bit; bit >>= 1) j -= bit;
		j += bit;
		if (i < j) std::swap(vB[i], vB[j]);
	}

	for (size_t length = 2; length <= vB.size(); length <<= 1)
	{
		float theta = 2.f * PI / length;
		Complex sample(std::cos(theta), std::sin(theta));
		for (size_t i = 0; i < vB.size(); i += length)
		{
			Complex current(1.f);
			for (size_t j = 0; j < length / 2; ++j)
			{
				Complex u = vB[i + j];
				Complex v = vB[i + j + length / 2] * current;
				vB[i + j] = u + v;
				vB[i + j + length / 2] = u - v;
				current *= sample;
			}
		}
	}
}

/**
 Calculates the samples using the FFT Series
 */
void Spectrum::calculateSamples()
{
	int offset = _audio.getPlayingOffset().asSeconds() * _buffer.getSampleRate() * _buffer.getChannelCount();
	for (size_t i = 0; (i < BUFFER_SIZE) && (i + offset < _buffer.getSampleCount()); ++i)
		_samples[i] = Complex(_buffer.getSamples()[i + offset] * _hammingWindow[i], 0.f);
}

/**
 Updates Spectrum ~ Private
 */
void Spectrum::updateSpectrum()
{
	// Clear
	_VA1.clear();
	_VA2.clear();

	// Update
	calculateSamples();
	FFT(_samples);

	// Set
	for (size_t i = 0; i < BUFFER_SIZE / 2 && i < 350; ++i)
	{
		float dB = std::fabs(_samples[i]);
		// Limiter, maybe make a compression function
		if (dB > 5000000.f)
			dB -= dB * .45f;
		// Cozy orange sf::Color(255, 140, 0)

		// Positive
		_VA1.append(sf::Vertex({SCREEN_WIDTH / 4.7f + 5.f * i, 95.f - dB / 1000000.f}, sf::Color(255, RAND_BETWEEN(120, 150), 0)));
		_VA2.append(sf::Vertex({SCREEN_HEIGHT / 4.7f + 5.f * i, 95.f - dB / 90000.f}, sf::Color(255, RAND_BETWEEN(120, 150), 0)));
		_VA1.append(sf::Vertex({SCREEN_WIDTH / 4.7f + 5.f * i + 3.f, 95.f - dB / 50000.f}, sf::Color::Red));
		_VA2.append(sf::Vertex({SCREEN_HEIGHT / 4.7f + 5.f * i, 95.f}, sf::Color::Red));

		// Negative
		_VA1.append(sf::Vertex({SCREEN_WIDTH / 4.7f + 5.f * i, 95.f}, sf::Color::Red));
		_VA2.append(sf::Vertex({SCREEN_HEIGHT / 4.7f + 5.f * i + 3.f, 95.f + dB / 70000.f}, sf::Color::Red));
		_VA1.append(sf::Vertex({SCREEN_WIDTH / 4.7f + 5.f * i, 95.f + dB / 90000.f}, sf::Color(255, RAND_BETWEEN(120, 150), 0)));
		_VA2.append(sf::Vertex({SCREEN_HEIGHT / 4.7f + 5.f * i, 95.f + dB / 1000000.f}, sf::Color(255, RAND_BETWEEN(120, 150), 0)));
	}
}

/**
 Default Constructor 
 */
Spectrum::Spectrum() : _samples(BUFFER_SIZE), _hammingWindow(new float[BUFFER_SIZE]),
					   _VA1(sf::TrianglesStrip), _VA2(sf::LinesStrip)
{
	_audio.setVolume(10.f);
	//initialize Ham window (and pov angle)
	for (size_t i = 0; i < BUFFER_SIZE; ++i)
		_hammingWindow[i] = .54f - .46f * std::cos((2.f * PI * i) / BUFFER_SIZE);
}

/**
 Deconstructor
 */
Spectrum::~Spectrum()
{
	_audio.stop();
	delete _hammingWindow;
	std::cout << "Spectrum Memory Deallocated" << std::endl;
}

/**
 Initialize The Spectrum
 @param- Song to show
 */
void Spectrum::setSong(const std::string &song)
{
	_buffer.loadFromFile(song);
	_audio.openFromFile(song);
}

void Spectrum::play(bool play) {play ? _audio.play() : _audio.stop();}
void Spectrum::stop() {_audio.stop();}

void Spectrum::draw(sf::RenderWindow &window)
{
	updateSpectrum();
	window.draw(_VA1);
	window.draw(_VA2);
}
