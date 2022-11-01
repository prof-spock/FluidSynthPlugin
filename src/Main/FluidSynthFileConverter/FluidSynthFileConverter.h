/**
 * @file
 * The <C>FluidSynthFileConverter</C> specification defines a
 * conversion program from a MIDI file into a wave file using the
 * FluidSynth library.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-08
 */

namespace Main::FluidSynthFileConverter {

    /**
     * Simple class wrapper around the main conversion function
     * converting a MIDI file into a wave file using a single sound
     * font with a command-line conforming to that of fluidsynth
     */
    struct ConverterProgram {

        /**
         * Converts a MIDI file into a wave file using a single sound font
         * with a command-line conforming to that of fluidsynth.
         *
         * @param[in] argc   number of command line arguments (including
         *                   command name)
         * @param[in] argv   the array of command line arguments
         * @return  0 on success, positive value on error
         */
        static int main (int argc, char* argv[]);

    };

}
