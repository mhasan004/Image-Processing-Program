// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2018 by George Wolberg
//
// HistoMatch.cpp - Histogram Match widget.
//
// Written by: George Wolberg, 2018
// ======================================================================

#include "MainWindow.h"
#include "HistoMatch.h"

extern MainWindow *g_mainWindowP;
bool	initLut(ImagePtr, int);
extern void HW_histoMatch(ImagePtr I1, ImagePtr targetHisto, bool approxAlg, ImagePtr I2);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HistoMatch::HistoMatch:
//
// Constructor.
//
HistoMatch::HistoMatch(QWidget *parent) : ImageFilter(parent)
{}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HistoMatch::controlPanel:
//
// Create group box for control panel.
//
QGroupBox*
HistoMatch::controlPanel()
{
	// init group box
	m_ctrlGrp = new QGroupBox("HistoMatch");

	// create label
	QLabel *label = new QLabel("Exponent");

	// create slider
	m_slider = new QSlider(Qt::Horizontal, m_ctrlGrp);
	m_slider->setRange(-10, 10);
	m_slider->setValue(0);
	m_slider->setTickPosition(QSlider::TicksBelow);
	m_slider->setTickInterval(2);

	// create spinbox
	m_spinBox = new QSpinBox(m_ctrlGrp);
	m_spinBox->setRange(-10, 10);
	m_spinBox->setValue(0);

	// create checkbox
	m_checkBox = new QCheckBox("Approximate" );
	m_checkBox-> setCheckState( Qt::Unchecked);

	// create lut for storing target histogram
	m_lut = IP_allocImage(MXGRAY, 1, INTCH_TYPE);

	// init signal/slot connections for slider and spinbox
	connect(m_slider  , SIGNAL(valueChanged(int)), this, SLOT(changeExp(int)));
	connect(m_spinBox , SIGNAL(valueChanged(int)), this, SLOT(changeExp(int)));
	connect(m_checkBox, SIGNAL(stateChanged(int)), this, SLOT(approxAlg(int)));

	// assemble dialog
	QGridLayout *layout = new QGridLayout;
	layout->addWidget(label    , 0, 0);
	layout->addWidget(m_slider , 0, 1);
	layout->addWidget(m_spinBox, 0, 2);
	layout->addWidget(m_checkBox, 1, 0, 1, 3, Qt::AlignLeft);

	// assign layout to group box
	m_ctrlGrp->setLayout(layout);

	return(m_ctrlGrp);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HistoMatch::applyFilter:
//
// Run filter on the image, transforming I1 to I2.
// Overrides ImageFilter::applyFilter().
// Return 1 for success, 0 for failure.
//
bool
HistoMatch::applyFilter(ImagePtr I1, bool /*gpuFlag*/, ImagePtr I2)
{
	// error checking
	if(I1.isNull()) return 0;

	// get exponent
	int exp = m_slider->value();

	// get approximation algorithm flag
	bool approxFlag = 0;
	if(m_checkBox->checkState() == Qt::Checked)
		approxFlag = 1;

	// apply filter
	initLut(I1, m_lut, exp);		// init lut with target histogram
	histoMatch(I1, m_lut, approxFlag, I2);	// histogram matching based on target histogram

	return 1;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HistoMatch::initLut:
//
// Apply histogram matching to I1. Output is in I2.
//
bool
HistoMatch::initLut(ImagePtr I, ImagePtr Ilut, int exp)
{
	// error checking
	if(   I.isNull()) return 0;
	if(Ilut.isNull()) return 0;

	int w = I->width ();
	int h = I->height();
	int total = w * h;
	int Havg = (float) total / MXGRAY;

	// get pointer lut data
	int type;
	ChannelPtr<int> lut;
	IP_getChannel(Ilut, 0, lut, type);

	// load lut with i^exp
	if(exp == 0) {
		for(int i=0; i<MXGRAY; i++)
			lut[i] = Havg;
	} else if(exp > 0) {
		for(int i=0; i<MXGRAY; i++)
			lut[i] = ROUND(pow((double) i/MaxGray, exp) * MaxGray);
	} else {
		for(int i=0; i<MXGRAY; i++)
			lut[i] = ROUND((1.0 - pow((double)i/MaxGray, -exp)) * MaxGray);
	}

	return 1;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HistoMatch::histoMatch:
//
// Apply histogram matching to I1. Output is in I2.
//
void
HistoMatch::histoMatch(ImagePtr I1, ImagePtr Ilut, bool approxFlag, ImagePtr I2)
{
	HW_histoMatch(I1, Ilut, approxFlag, I2);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HistoMatch::changeExp:
//
// Slot to process change in thr1 caused by moving the slider.
//
void
HistoMatch::changeExp(int exp)
{
	// set slider and spinbox to same value
	m_slider ->blockSignals(true);
	m_slider ->setValue    (exp);
	m_slider ->blockSignals(false);
	m_spinBox->blockSignals(true);
	m_spinBox->setValue    (exp);
	m_spinBox->blockSignals(false);

	// apply filter to source image and display result
	g_mainWindowP->preview();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HistoMatch::approxAlg:
//
// Slot to process the checkbox that enables/disables the use of the approximate algorithm.
//
void
HistoMatch::approxAlg(int flag)
{
	int checkState = m_checkBox->checkState();

	if(flag && checkState != Qt::Checked)
		m_checkBox->setCheckState (Qt::Checked);
	else if(!flag && checkState == Qt::Checked)
		m_checkBox->setCheckState (Qt::Unchecked);

	// apply filter and display output
	g_mainWindowP->preview();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HistoMatch::reset:
//
// Reset parameters.
//
void
HistoMatch::reset()
{
	m_slider ->blockSignals(true);
	m_slider ->setValue    (0);
	m_slider ->blockSignals(false);
	m_spinBox->blockSignals(true);
	m_spinBox->setValue    (0);
	m_spinBox->blockSignals(false);

	// apply filter to source image and display result
	g_mainWindowP->preview();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HistoMatch::initShader:
//
// init shader program and parameters.
//
void
HistoMatch::initShader() {

	// flag to indicate shader availability
	m_shaderFlag = false;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HistoMatch::gpuProgram:
//
// Invoke GPU program
//
void
HistoMatch::gpuProgram(int) {}
