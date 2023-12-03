/// @file

#include <iostream>
#include <fstream>
#include <string.h>
#include <errno.h>

#include "wavfile_mono.h"
#include "pitch_analyzer.h"

#include "docopt.h"

#define FRAME_LEN 0.030   /* 30 ms. */
#define FRAME_SHIFT 0.015 /* 15 ms. */

using namespace std;
using namespace upc;

static const char USAGE[] = R"(
get_pitch - Pitch Estimator 

Usage:
    get_pitch [options] <input-wav> <output-txt>
    get_pitch (-h | --help)
    get_pitch --version

Options:
    -p, --u_pot=REAL   Umbral de potencia para la terminaciÃ³n de sonoro sordo [default: -48]
    -1, --u_r1=REAL    Umbral de la autocorrelaciÃ³n de 1 para sonoro-sordo [default: 0.95]
    -m, --u_rmax=REAL  Umbral en el mÃ¡ximo de la autocorrelaciÃ³n [default: 0.2]
    -z, --u_zcr=FLOAT   Umbral cruces por zero [default: 2300]
    -h, --help  Show this screen
    --version   Show the version of the project

Arguments:
    input-wav   Wave file with the audio signal
    output-txt  Output file: ASCII file with the result of the estimation:
                    - One line per frame with the estimated f0
                    - If considered unvoiced, f0 must be set to f0 = 0
)";

int main(int argc, const char *argv[])
{
  /// \TODO
  ///  Modify the program syntax and the call to **docopt()** in order to
  ///  add options and arguments to the program.
  std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
                                                             {argv + 1, argv + argc}, // array of arguments, without the program name
                                                             true,                    // show help if requested
                                                             "2.0");                  // version string

  std::string input_wav = args["<input-wav>"].asString();
  std::string output_txt = args["<output-txt>"].asString();
  float u_pot = stof(args["--u_pot"].asString());
  float u_r1 = stof(args["--u_r1"].asString());
  float u_rmax = stof(args["--u_rmax"].asString());
  float u_zcr = stof(args["--u_zcr"].asString());

  // Read input sound file
  unsigned int rate;
  vector<float> x;
  if (readwav_mono(input_wav, rate, x) != 0)
  {
    cerr << "Error reading input file " << input_wav << " (" << strerror(errno) << ")\n";
    return -2;
  }

  // hacer normalización de la potencia (x)

  int n_len = rate * FRAME_LEN;
  int n_shift = rate * FRAME_SHIFT;

  // Define analyzer
  PitchAnalyzer analyzer(n_len, rate, PitchAnalyzer::RECT, 50, 500);

  /// \TODO
  /// Preprocess the input signal in order to ease pitch estimation. For instance,
  /// central-clipping or low pass filtering may be used.

  // Iterate for each frame and save values in f0 vector
  vector<float>::iterator iX;
  vector<float> f0;
  vector<float> f0_aux;
  for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift)
  {
    float f = analyzer(iX, iX + n_len);
    f0.push_back(f);
  }

  /// \TODO
  /// Postprocess the estimation in order to supress errors. For instance, a median filter
  /// or time-warping may be used.
  unsigned int aux = 0;
  vector<float>::iterator post;
  vector<float> mediana(3, 0); 
  for (post = f0_aux.begin(); post != f0_aux.end(); ++post)
  {

    mediana.assign({*prev(post, 1), *post, *next(post, 1)}); 
    sort(mediana.begin(), mediana.end());                    

    if (mediana.at(0) <= mediana.at(1) / 2 || mediana.at(2) >= 2 * mediana.at(1))
    {                             
      f0.at(aux) = mediana.at(1); 
    }

    aux++;
  } /// \DONE Post procesado - Realizadao con filtro de mediana de longitud 3

  float llindar = 0.008; // encontrar valor óptimo
  for (unsigned int n = 0; n < x.size(); n++)
  {
    if (x[n] < llindar && x[n] > -llindar)
    {
      x[n] = 0;
    }
  } /// \DONE Center clipping

  // Write f0 contour into the output file
  ofstream os(output_txt);
  if (!os.good())
  {
    cerr << "Error reading output file " << output_txt << " (" << strerror(errno) << ")\n";
    return -3;
  }

  os << 0 << '\n'; // pitch at t=0
  for (iX = f0.begin(); iX != f0.end(); ++iX)
    os << *iX << '\n';
  os << 0 << '\n'; // pitch at t=Dur

  return 0;
}
