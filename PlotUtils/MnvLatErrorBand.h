#ifndef MNV_MnvLatErrorBand_H
#define MNV_MnvLatErrorBand_H 1

#include "TObject.h"
#include "TString.h"
#include "TError.h"
#include "TNamed.h"
#include "TH1D.h"
#include "TMatrixD.h"
#include "TMatrixDBase.h"

#include <assert.h>
#include <stdexcept>
#include <vector>
#include <string>
#include <iostream>
#include <math.h>

namespace PlotUtils
{

  /*! @brief Create a systematic error band and covariance matrix using the many universes method where universes differ in a lateral shift amount.

    Error in each bin is taken to be:
    <ul>
    <li>the maximum among universes (including central value) for small numbers of universes ( nUniverses < 10 ).
    <li>The standard deviation among universes for large numbers of universes (nUniverses >= 10 )
    </ul>
    Normally the lateral shift has two universes for +/-1 sigma shifts.

    @author Brian Tice, Gabriel Perdue
   */

  class MnvLatErrorBand : public TH1D
  {
    public:
      //! Default constructor
      MnvLatErrorBand( ) : TH1D() {};

      //==== Copy Constructors from TH1D ====//
      //! Construct from vector 
      /*
         MnvLatErrorBand( const TVectorD& v ) : TH1D(v) {};

      //! Copy constructor
      MnvLatErrorBand( const TH1D& h1d ) : TH1D(h1d) {};

      //! Construct with variable bin sizes
      MnvLatErrorBand( const char* name, const char* title, Int_t nbinsx, const Float_t* xbins): TH1D(name,title,nbinsx,xbins) {} ;

      //! Construct with variable bin sizes
      MnvLatErrorBand( const char* name, const char* title, Int_t nbinsx, const Double_t* xbins): TH1D(name,title,nbinsx,xbins) {} ;

      //! Construct with constant bin sizes
      MnvLatErrorBand( const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup) : TH1D(name,title,nbinsx,xlow,xup) {} ;
       */


      /*! Standard Constructor 
        @param[in] name Name the error band
        @param[in] base Pointer to histogram we are applying an error band to
        @param[in] nHists Number of universes.  Default=2, expected to be +/-1 sigma from central value
       */
      MnvLatErrorBand( const std::string& name, const TH1D* base, const unsigned int nHists = 2 );

      //! Add a new Constructor for already Filled vector of Histogram Error Bands
      MnvLatErrorBand( const std::string& name, const TH1D* base, const std::vector<TH1D*>& hists );

      //! Deep copy constructor
      MnvLatErrorBand( const MnvLatErrorBand& h );

      //! Deep assignment operator
      MnvLatErrorBand& operator=( const MnvLatErrorBand& h );

    private:
      //! A helper function which sets variables for the deep copy and assignment
      void DeepCopy( const MnvLatErrorBand& h );

    public:

      //!Destructor
      virtual ~MnvLatErrorBand();

      //! Fill the CVHist and all the universes' histos
      virtual bool Fill( const double val, const double *shifts, const double cvweight = 1.0, const bool fillcv = true, const double *weights = 0);

      /*! Fill the CV histo and 2 universes with these shifts
        Only works if nUniverses = 2 (assumed to be +/- nSigma)
        @param[in] val Central value to fill
        @param[in] shiftDown Distance from the cetral value for the symmetric -nSigma shift
        @param[in] shiftUp Distance from the cetral value for the symmetric +nSigma shift
        @param[in] cvweight Central value weight
       */
      virtual bool Fill( const double val, const double shiftDown, const double shiftUpconst, double cvweight = 1.0, const bool fillcv = true);

      //! Get the error band histogram
      virtual TH1D GetErrorBand( bool asFrac = false , bool cov_area_normalize = false) const;

      //! Will the error band come from max spread?
      bool GetUseSpreadError() const { return fUseSpreadError; };

      //! Set the error band to come from max spread or standard dev
      void SetUseSpreadError( bool use ) { fUseSpreadError = use; };

      //! How many histograms (universes) does this have?
      unsigned int GetNHists() const { return fNHists; };

      //! Get the universes' histograms (const)
      const std::vector<TH1D*>& GetHists() const { return fHists; };
      //! Get the universes' histograms (nonconst)
      std::vector<TH1D*> GetHists() { return fHists; };

      //! Get a specific universe's histogram (const)
      const TH1D* GetHist(const unsigned int i) const;
      //! Get a specific universe's histogram (nonconst)
      TH1D* GetHist(const unsigned int i);

      //! Draw all the histograms, including CVHist if the option is present
      void DrawAll( const char *option = "", bool drawCV = false, bool area_normalize = false, double normBinWidth = 0.0 ) const;

      //! Retrieve the weight for a single universe.  If no universes have weights present, will return -1.
      double GetUnivWgt(unsigned int uid) const;

      //! Retrieve all the weights together.  If no universes have weights present, will return NULL.
      const std::vector<double> * GetUnivWgts() const;

      //! Set the weight for a single universe.  (If other universes' weights are as-yet unspecified, they are set to 1.)
      void SetUnivWgt(unsigned int uid, double wgt);

      //! Set all universe weights simultaneously.  Must set exactly fNHists universes (or an exception will be thrown).
      void SetUnivWgts(const std::vector<double>& wgts);

      //! Scale the CV, and optionally all universes, by some constant using TH1D::Scale
      void Scale( Double_t c1 = 1., Option_t *option = "", Bool_t allUniv = true );

      /*! Replace this MnvLatErrorBand's contents with the result of a division of two other MnvLatErrorBands
        see http://root.cern.ch/root/html/TH1.html#TH1:Divide@2
       */
      Bool_t Divide( const MnvLatErrorBand* h1, const MnvLatErrorBand* h2, Double_t c1 = 1, Double_t c2 = 1, Option_t* option="" );

      /*! Divide this MnvLatErrorBand's by a function using TH1D::Divide().  Re-implements the TH1D method.
       */
      virtual Bool_t Divide( TF1 *f1, Double_t c1 = 1 );

      /*! Replace this MnvLatErrorBand's contents with the result of a division of one MnvLatErrorBand by a TH1
       */
      Bool_t DivideSingle( const MnvLatErrorBand* h1, const TH1* h2, Double_t c1 = 1, Double_t c2 = 1, Option_t* option="" );

      //! Replace this MnvLatErrorBand's contents with the result of a multiplication of two other MnvLatErrorBands
      Bool_t Multiply( const MnvLatErrorBand* h1, const MnvLatErrorBand* h2, Double_t c1 = 1, Double_t c2 = 1 );

      //! Replace this MnvLatErrorBand's contents with the result of a multiplication of another MnvLatErrorband and a TH1
      Bool_t MultiplySingle( const MnvLatErrorBand* h1, const TH1* h2, Double_t c1 = 1, Double_t c2 = 1 );

      //! Add a histogram to all universes
      Bool_t AddSingle( const TH1* h1, const Double_t c1 = 1. );

      //! Add MnvLatErrorband*c1 to this error band
      Bool_t Add( const MnvLatErrorBand* h1, const Double_t c1 = 1. );

      //! Rebin this error band
      MnvLatErrorBand * Rebin( const Int_t ngroup = 2, const Double_t *xbins = 0 );

      //! Reset all histograms known to this error band
      void Reset( Option_t *option = "" );

      //! Set/Reset histogram bit and propagate to universes
      void SetBit(UInt_t f, Bool_t set);
      //! Set histogram bit and propagate to universes
      void SetBit(UInt_t f) { SetBit(f, true); };


      //! Calculate Covariance Matrix
      TMatrixD CalcCovMx(bool area_normalize = false, bool asFrac = false) const;

      //! Calculate Correlation Matrix
      TMatrixD CalcCorrMx(bool area_normalize = false) const;

    protected:
      unsigned int fNHists;          ///< Number of histograms (universes)
      bool fUseSpreadError;          ///< Are we using spread in histos to get the error
      std::vector<double> fUnivWgts; ///< Weights for the universes.  1 is used for each universe if this goes unfilled.
      std::vector<TH1D*> fHists;     ///< Vector of histograms for the universes
      std::vector<int> fGoodColors;  ///< Current list of good colors to use for universe histos

    private:
      // adds the necessary ROOT garbledygook to make this class persistable
      ClassDef( MnvLatErrorBand, 4);
  }; //end of MnvLatErrorBand

} //end of PlotUtils

#endif
