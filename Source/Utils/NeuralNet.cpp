//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
//
// NeuralNet.cpp
//
// - neural net with backpropagation algorithm
//
// original version by: Karsten Kutza, 4/17/96 
//
#include "stdafx.h"
#include "NeuralNet.h"

IMPLEMENT_SERIAL(CNeuralNet, CObject, VERSIONABLE_SCHEMA | tkGenesis)


// default values
const double tfDefaultAlpha = 0.9;
const double tfDefaultEta = 0.25;
const double tfDefaultGain = 1;

//
int	CNeuralNet::m_nMajorVersion	= 1;
int	CNeuralNet::m_nMinorVersion	= 0;



/******************************************************************************
        R A N D O M S   D R A W N   F R O M   D I S T R I B U T I O N S
 ******************************************************************************/


int CNeuralNet::RandomInt(int Low, int High)
{
	return rand() % (High-Low+1) + Low;
}      


NVALUE CNeuralNet::RandomReal(double Low, double High)
{
	return (NVALUE) (((double) rand() / RAND_MAX) * (High-Low) + Low);
}      



/******************************************************************************
 *                       Construction / Destruction                           *
 ******************************************************************************/

// default constructor
CNeuralNet::CNeuralNet(int numInputs, int numOutputs, int numHiddenLayers, int numUnitsPerHiddenLayer) :
				m_numInputs(numInputs), 
				m_numOutputs(numOutputs),
				m_numHiddenLayers(numHiddenLayers),
				m_numLayers(numHiddenLayers+2),
				m_numUnitsPerHiddenLayer(numUnitsPerHiddenLayer)
{
	// set unit counts data
	m_numUnitsPerLayer.Add(m_numInputs);
	for(int i=0;i<m_numHiddenLayers;i++)
		m_numUnitsPerLayer.Add(m_numUnitsPerHiddenLayer);
	m_numUnitsPerLayer.Add(m_numOutputs);

	// and alloc memory
	GenerateNetwork();

	// init settings for new network
	m_fAlpha       = tfDefaultAlpha;
	m_fEta         = tfDefaultEta;
	m_fGain        = tfDefaultGain;
	//
	m_nFileMajorVersion = m_nMajorVersion;
	m_nFileMinorVersion = m_nMinorVersion;
	m_numTrainingCycles = 0;
}

// empty constructor used with serialization
CNeuralNet::CNeuralNet()
{
	// clear memory
	for(int i=0;i<tnumMaxLayers;i++)
		m_pLayers[i] = NULL;
}

// destructor
CNeuralNet::~CNeuralNet()
{
	// free the memory
	for (int nLayer=0; nLayer<m_numLayers; nLayer++) 
	{
		delete[] m_pLayers[nLayer]->pfOutputs;
		delete[] m_pLayers[nLayer]->pfError;
		//
		if (nLayer != 0) 
		{
			for (int i=1; i<=m_numUnitsPerLayer[nLayer]; i++) 
			{
				delete[] m_pLayers[nLayer]->ppfWeight[i];
				delete[] m_pLayers[nLayer]->ppfWeightSave[i];
				delete[] m_pLayers[nLayer]->ppfDeltaWeight[i];
			}
		}
		//
		free(m_pLayers[nLayer]->ppfWeight);
		free(m_pLayers[nLayer]->ppfWeightSave);
		free(m_pLayers[nLayer]->ppfDeltaWeight);
/*
		delete[] m_pLayers[nLayer]->ppfWeight;
		delete[] m_pLayers[nLayer]->ppfWeightSave;
		delete[] m_pLayers[nLayer]->ppfDeltaWeight;
*/
		//
		delete m_pLayers[nLayer];
	}
}



//
void CNeuralNet::Serialize(CArchive& ar)
{
	if (ar.IsLoading())
	{
		// load net info
		ar >> m_nFileMajorVersion;
		ar >> m_nFileMinorVersion;
		ar >> m_numTrainingCycles;
		ar >> m_numInputs;
		ar >> m_numOutputs;
		ar >> m_numLayers;
		ar >> m_numHiddenLayers;
		ar >> m_numUnitsPerHiddenLayer;
		ar >> m_fAlpha;
		ar >> m_fEta;
		ar >> m_fGain;

		// init layers data
		m_numUnitsPerLayer.Add(m_numInputs);
		for(int i=0;i<m_numHiddenLayers;i++)
			m_numUnitsPerLayer.Add(m_numUnitsPerHiddenLayer);
		m_numUnitsPerLayer.Add(m_numOutputs);

		// create the net
		ASSERT(m_pLayers[0] == NULL);
		GenerateNetwork();

		// then load the net
		// iterate over the layers
		for (int nLayer=1; nLayer<m_numLayers; nLayer++) 
		{
			// iterate over the nodes
			for (int i=1; i<=m_numUnitsPerLayer[nLayer]; i++) 
			{
				// iterate over the connections to the previous layer
				for(int j=0;j<m_numUnitsPerLayer[nLayer-1]+1;j++)
				{
					ar >> m_pLayers[nLayer]->ppfWeight[i][j];
					ar >> m_pLayers[nLayer]->ppfWeightSave[i][j];
					ar >> m_pLayers[nLayer]->ppfDeltaWeight[i][j];
				}
			}
		}
	}
	else
	{
		// save parameters
		ar << m_nFileMajorVersion;
		ar << m_nFileMinorVersion;
		ar << m_numTrainingCycles;
		ar << m_numInputs;
		ar << m_numOutputs;
		ar << m_numLayers;
		ar << m_numHiddenLayers;
		ar << m_numUnitsPerHiddenLayer;
		ar << m_fAlpha;
		ar << m_fEta;
		ar << m_fGain;

		// check version

		// save the net 
		// iterate over the layers
		for (int nLayer=1; nLayer<m_numLayers; nLayer++) 
		{
			// iterate over the nodes
			for (int i=1; i<=m_numUnitsPerLayer[nLayer]; i++) 
			{
				// iterate over the connections to the previous layer
				for(int j=0;j<m_numUnitsPerLayer[nLayer-1]+1;j++)
				{
					ar << m_pLayers[nLayer]->ppfWeight[i][j];
					ar << m_pLayers[nLayer]->ppfWeightSave[i][j];
					ar << m_pLayers[nLayer]->ppfDeltaWeight[i][j];
				}
			}
		}
	}
}



/******************************************************************************
 *                        I N I T I A L I Z A T I O N                         *
 ******************************************************************************/



//
// GenerateNetwork()
//
void CNeuralNet::GenerateNetwork()
{
	for (int nLayer=0; nLayer<m_numLayers; nLayer++) 
	{
		m_pLayers[nLayer] = (LAYER*) malloc(sizeof(LAYER));

		m_pLayers[nLayer]->numUnits			= m_numUnitsPerLayer[nLayer];
		m_pLayers[nLayer]->pfOutputs		= new NVALUE[m_numUnitsPerLayer[nLayer]+1];
		m_pLayers[nLayer]->pfError			= new NVALUE[m_numUnitsPerLayer[nLayer]+1];
		m_pLayers[nLayer]->ppfWeight		= new NVALUE*[m_numUnitsPerLayer[nLayer]+1];
		m_pLayers[nLayer]->ppfWeightSave	= new NVALUE*[m_numUnitsPerLayer[nLayer]+1];
		m_pLayers[nLayer]->ppfDeltaWeight	= new NVALUE*[m_numUnitsPerLayer[nLayer]+1];
		m_pLayers[nLayer]->pfOutputs[0]	= BIAS;

		if (nLayer != 0) 
		{
			for (int i=1; i<=m_numUnitsPerLayer[nLayer]; i++) 
			{
				m_pLayers[nLayer]->ppfWeight[i]	  = (NVALUE*) calloc(m_numUnitsPerLayer[nLayer-1]+1, sizeof(NVALUE));
				m_pLayers[nLayer]->ppfWeightSave[i]  = (NVALUE*) calloc(m_numUnitsPerLayer[nLayer-1]+1, sizeof(NVALUE));
				m_pLayers[nLayer]->ppfDeltaWeight[i] = (NVALUE*) calloc(m_numUnitsPerLayer[nLayer-1]+1, sizeof(NVALUE));
/*
				m_pLayers[nLayer]->ppfWeight[i]		 = new NVALUE[m_numUnitsPerLayer[nLayer-1]+1];
				m_pLayers[nLayer]->ppfWeightSave[i]  = new NVALUE[m_numUnitsPerLayer[nLayer-1]+1];
				m_pLayers[nLayer]->ppfDeltaWeight[i] = new NVALUE[m_numUnitsPerLayer[nLayer-1]+1];
*/
			}
		}
	}

	//
	m_pInputLayer  = m_pLayers[0];
	m_pOutputLayer = m_pLayers[m_numLayers - 1];
}



//
// RandomizeWeights()
//
void CNeuralNet::RandomizeWeights()
{
	for (int nLayer=1; nLayer<m_numLayers; nLayer++) 
	{
		for (int i=1; i<=m_pLayers[nLayer]->numUnits; i++) 
		{
			for (int j=0; j<=m_pLayers[nLayer-1]->numUnits; j++)
				m_pLayers[nLayer]->ppfWeight[i][j] = (NVALUE) RandomReal(-0.5, 0.5);
		}
	}

	// clear out error values
	for (nLayer=1; nLayer<m_numLayers; nLayer++) 
	{
		for (int i=1; i<=m_pLayers[nLayer]->numUnits; i++) 
			m_pLayers[nLayer]->pfError[i] = 0;
	}
}



//
// SetInputs()
//
void CNeuralNet::SetInputs(NVALUE* pfInputs)
{
	for (int i=1; i<=m_pInputLayer->numUnits; i++)
		m_pInputLayer->pfOutputs[i] = pfInputs[i-1];
}



//
// GetOutputs()
//
void CNeuralNet::GetOutputs(NVALUE* pfOutputs)
{
	for (int i=1; i<=m_pOutputLayer->numUnits; i++)
		pfOutputs[i-1] = m_pOutputLayer->pfOutputs[i];
}


/******************************************************************************
            S U P P O R T   F O R   S T O P P E D   T R A I N I N G
 ******************************************************************************/


//
// SaveWeights()
//
void CNeuralNet::SaveWeights()
{
	for (int nLayer=1; nLayer<m_numLayers; nLayer++) 
	{
		for (int i=1; i<=m_pLayers[nLayer]->numUnits; i++) 
		{
			for (int j=0; j<=m_pLayers[nLayer-1]->numUnits; j++)
				m_pLayers[nLayer]->ppfWeightSave[i][j] = m_pLayers[nLayer]->ppfWeight[i][j];
		}
	}
}


//
// RestoreWeights()
//
void CNeuralNet::RestoreWeights()
{
	for (int nLayer=1; nLayer<m_numLayers; nLayer++) 
	{
		for (int i=1; i<=m_pLayers[nLayer]->numUnits; i++) 
		{
			for (int j=0; j<=m_pLayers[nLayer-1]->numUnits; j++)
				m_pLayers[nLayer]->ppfWeight[i][j] = m_pLayers[nLayer]->ppfWeightSave[i][j];
		}
	}
}


/******************************************************************************
                     P R O P A G A T I N G   S I G N A L S
 ******************************************************************************/


//
// PropagateLayer()
//
void CNeuralNet::PropagateLayer(LAYER* Lower, LAYER* Upper)
{
	int  i,j;
	NVALUE Sum;

	for (i=1; i<=Upper->numUnits; i++) 
	{
		Sum = 0;
		for (j=0; j<=Lower->numUnits; j++)
			Sum += Upper->ppfWeight[i][j] * Lower->pfOutputs[j];
		
		Upper->pfOutputs[i] = (NVALUE) (1 / (1 + exp(-m_fGain * Sum)));
	}
}



//
// PropagateNet()
//
void CNeuralNet::PropagateNet()
{
	for (int nLayer=0; nLayer<m_numLayers-1; nLayer++)
		PropagateLayer(m_pLayers[nLayer], m_pLayers[nLayer+1]);
}


/******************************************************************************
                  B A C K P R O P A G A T I N G   E R R O R S
 ******************************************************************************/



//
// void ComputeOutputError()
//
void CNeuralNet::ComputeOutputError(NVALUE* pfTargets, int nOffset, int nBlockSize)
{
	int  i;
	NVALUE Out, Err;
	int nEnd = (nBlockSize > 0)? nOffset+nBlockSize-1 : m_pOutputLayer->numUnits;

	m_fError = 0;
	for (i=nOffset; i<=nEnd; i++) 
	{
		Out = m_pOutputLayer->pfOutputs[i];
		Err = pfTargets[i-1] - Out;
		m_pOutputLayer->pfError[i] = (NVALUE) (m_fGain * Out * (1-Out) * Err);
		m_fError += 0.5 * sqr(Err);
	}
}



//
// BackpropagateLayer()
//
void CNeuralNet::BackpropagateLayer(LAYER* Upper, LAYER* Lower, int nOffset, int nBlockSize)
{
	NVALUE Out, Err;
	int nEnd = (nBlockSize > 0)? nOffset+nBlockSize-1 : Upper->numUnits;
	//
	for (int i=0; i<Lower->numUnits; i++) 
	{
		Out = Lower->pfOutputs[i];
		Err = 0;
		for (int j=nOffset; j<=nEnd; j++) 
			Err += Upper->ppfWeight[j][i] * Upper->pfError[j];

		Lower->pfError[i] = (NVALUE) (m_fGain * Out * (1-Out) * Err);
	}
}



//
// BackpropagateNet()
//
// - backpropagate errors after a run
//
void CNeuralNet::BackpropagateNet(int nOffset, int nBlockSize)
{
	if (nBlockSize > 0)
	{
		BackpropagateLayer(m_pLayers[m_numLayers-1], m_pLayers[m_numLayers-2], nOffset, nBlockSize);
		for (int nLayer=m_numLayers-2; nLayer>1; nLayer--)
			BackpropagateLayer(m_pLayers[nLayer], m_pLayers[nLayer-1]);
	}
	else
	{
		for (int nLayer=m_numLayers-1; nLayer>1; nLayer--)
			BackpropagateLayer(m_pLayers[nLayer], m_pLayers[nLayer-1]);
	}
}



//
// AdjustWeights()
//
// - adjust the weights after a run
//
void CNeuralNet::AdjustWeights(int nOffset, int nBlockSize)
{
	NVALUE Out, Err, ppfDeltaWeight;

	for (int nLayer=1; nLayer<m_numLayers; nLayer++) 
	{
		int nStart = 1;
		int nEnd = m_pLayers[nLayer]->numUnits;
		if (nLayer == m_numLayers-1)
		{
			nStart = nOffset;
			nEnd = nOffset + nBlockSize - 1;
		}
		for (int i=nStart; i<=nEnd; i++) 
		{
			for (int j=0; j<=m_pLayers[nLayer-1]->numUnits; j++) 
			{
				Out = m_pLayers[nLayer-1]->pfOutputs[j];
				Err = m_pLayers[nLayer]->pfError[i];
				ppfDeltaWeight = m_pLayers[nLayer]->ppfDeltaWeight[i][j];
				m_pLayers[nLayer]->ppfWeight[i][j] += (NVALUE) (m_fEta * Err * Out + m_fAlpha * ppfDeltaWeight);
				m_pLayers[nLayer]->ppfDeltaWeight[i][j] = (NVALUE) (m_fEta * Err * Out);
			}
		}
	}
}



/******************************************************************************
 *
 *                Public Operations
 *
 ******************************************************************************/


//
// SimulateNet()
//
// - run a set of inputs through the net and get the results
//
void CNeuralNet::SimulateNet(NVALUE* pfInputs, NVALUE* pfOutputs)
{
	SetInputs(pfInputs);
	PropagateNet();
	GetOutputs(pfOutputs);
}



//
// TrainNet()
//
// - adjusts the net towards the correct result through backpropagaton 
//   and weights adjustment
//
void CNeuralNet::TrainNet(NVALUE* pfTargets, int nOffset, int nBlockSize)
{
	// account for bias term
	nOffset++;
	//
	ComputeOutputError(pfTargets, nOffset, nBlockSize);
	BackpropagateNet(nOffset, nBlockSize);
	AdjustWeights(nOffset, nBlockSize);
	//
	m_numTrainingCycles++;
}
