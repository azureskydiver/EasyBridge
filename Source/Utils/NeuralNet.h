//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Neural Net.h
//
#ifndef __NEURALNET_BP__
#define __NEURALNET_BP__

//
#include <math.h>

typedef float	NVALUE;

//
#define MIN_REAL      -HUGE_VAL
#define MAX_REAL      +HUGE_VAL
//#define MIN(x,y)      ((x)<(y) ? (x) : (y))
//#define MAX(x,y)      ((x)>(y) ? (x) : (y))

//
const int tnumMaxLayers	= 10;

#define LO            0.1
#define HI            0.9
#define BIAS          1

#define sqr(x)        ((x)*(x))

typedef struct {				/* A LAYER OF A NET:                     */
    int			numUnits;		/* - number of units in this layer       */
    NVALUE*		pfOutputs;		/* - output of ith unit                  */
    NVALUE*		pfError;		/* - error term of ith unit              */
    NVALUE**	ppfWeight;		/* - connection weights to ith unit      */
    NVALUE**	ppfWeightSave;	/* - saved weights for stopped training  */
    NVALUE**	ppfDeltaWeight;	/* - last weight deltas for momentum     */
} LAYER;


//
class AFX_EXT_CLASS CNeuralNet : public CObject
{
//
// operations
//
public:
	// construction/destruction
	CNeuralNet(int numInputs, int numOutputs, int numHiddenLayers, int numUnitsPerHiddenLayer);
	CNeuralNet();	// default constructor
	~CNeuralNet();

	// versions
	enum {
		tkGenesis,
		tkVersion1
	} NNetVersion;

	// serialization support
	DECLARE_SERIAL(CNeuralNet)
	virtual void Serialize(CArchive& ar);

	// utility functions
	virtual void GenerateNetwork();
	virtual void RandomizeWeights();
	virtual void SetInputs(NVALUE* pfInputs);
	virtual void GetOutputs(NVALUE* pfOutputs);
	virtual void SaveWeights();
	virtual void RestoreWeights();
	//
	static int    RandomInt(int Low, int High);
	static NVALUE RandomReal(double Low, double High);

	// inline operations
	int GetNumInputs() const { return m_numInputs; }
	int GetNumOutputs() const { return m_numOutputs; }
	int GetNumLayers() const { return m_numLayers; }
	int GetNumHiddenLayers() const { return m_numHiddenLayers; }
	int GetNumUnitsPerHiddenLayer() const { return m_numUnitsPerHiddenLayer; }
	int	GetFileMajorVersion() const { return m_nFileMajorVersion; }
	int GetFileMinorVersion() const { return m_nFileMinorVersion; }
	int GetNumTrainingCycles() const { return m_numTrainingCycles; }
	//
    void SetAlpha(double fValue) { m_fAlpha = fValue; }
    double GetAlpha() const { return m_fAlpha; }
    void SetEta(double fValue) { m_fEta = fValue; }
    double GetEta(double fValue) const { return m_fEta; }
    void SetGain(double fValue) { m_fGain = fValue; }
    double GetGain() const { return m_fEta; }
    double GetError() const { return m_fError; }

	// the actual simulation & training routines
	virtual void SimulateNet(NVALUE* pfInputs, NVALUE* pfOutputs);
	virtual void TrainNet(NVALUE* pfTargets, int nOffset=0, int nBlockSize=0);


protected:
	virtual void PropagateLayer(LAYER* Lower, LAYER* Upper);
	virtual void PropagateNet();
	virtual void ComputeOutputError(NVALUE* pfTargets, int nOffset=0, int nBlockSize=0);
	virtual void BackpropagateLayer(LAYER* Upper, LAYER* Lower, int nOffset=1, int nBlockSize=0);
	virtual void BackpropagateNet(int nOffset=1, int nBlockSize=0);
	virtual void AdjustWeights(int nOffset=0, int nBlockSize=0);


// private operations
private:


//
// data
//
public:
	static int		m_nMajorVersion;
	static int		m_nMinorVersion;

//
private:
	// topology
	int				m_numLayers;
	int				m_numHiddenLayers;
	int				m_numUnitsPerHiddenLayer;
	int				m_numInputs;
	int				m_numOutputs;
	CArray<int,int>	m_numUnitsPerLayer;

	// net info
    LAYER*		m_pLayers[tnumMaxLayers];	// layers of this net
    LAYER*		m_pInputLayer;			// input layer
    LAYER*		m_pOutputLayer;		// output layer
    double		m_fAlpha;			// momentum factor
    double		m_fEta;				// learning rate
    double		m_fGain;			// gain of sigmoid function
    double		m_fError;			// total net error
	//
	int			m_nFileMajorVersion;
	int			m_nFileMinorVersion;
	int			m_numTrainingCycles;
};


#endif