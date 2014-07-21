#include "complex_forward_fourier_transformer.hh"

namespace midge
{

    complex_forward_fourier_transformer::complex_forward_fourier_transformer() :
            f_plan()
    {
    }
    complex_forward_fourier_transformer::~complex_forward_fourier_transformer()
    {
    }

    void complex_forward_fourier_transformer::initialize_transformer()
    {
        if( input< 0 >()->get_size() != output< 0 >()->get_size() )
        {
            throw error() << "complex forward fourier transformer named <" << this->get_name() << "> must have matching input and output sizes";
        }

        f_plan = fftw_plan_dft_1d( input< 0 >()->get_size(), input< 0 >()->raw(), output< 0 >()->raw(), FFTW_FORWARD, FFTW_MEASURE );

        return;
    }
    void complex_forward_fourier_transformer::execute_transformer()
    {
        fftw_execute( f_plan );

        return;
    }
    void complex_forward_fourier_transformer::finalize_transformer()
    {
        fftw_destroy_plan( f_plan );

        return;
    }

}
