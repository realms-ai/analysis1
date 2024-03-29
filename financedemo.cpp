#include <iostream>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>                                                              
#include <time.h>  
#include <string.h>
#include <QApplication>
#include <vector>
#include <sstream>
#include <algorithm>
#include <QHBoxLayout>
#include <QObjectList>
#include <QDateTime>
#include <QPushButton>
#include <QButtonGroup>
#include <QMouseEvent>
#include <mysql++.h>
#include <typeinfo>
#include "chartdir.h"
#include "FinanceChart.h"
#include "financedemo.h"

using namespace std;
using namespace mysqlpp;

int main(int argc, char *argv[])
{    
    cout << "main \n";
    QApplication app(argc, argv);
    FinanceDemo demo;
    demo.show();
    return app.exec();
}


//
// Because QT uses QDateTime, while ChartDirector uses Chart::chartTime, we need
// utilities to convert from one to another
//

// Convert from QDateTime to chartTime
/*static double QDateTimeToChartTime(QDateTime q)
{
    cout << "QDateTimeToChartTime \n";
    QDate d = q.date();
    QTime t = q.time();
    return Chart::chartTime(d.year(), d.month(), d.day(), t.hour(), t.minute(),
        t.second()) + t.msec() / 1000.0;
}*/

// Convert from chartTime to QDateTime
/*static QDateTime ChartTimeToQDateTime(double t)
{
    double ymdhms = floor(t);
    int ms = (int)(floor((t - ymdhms) * 1000));
    int ymd = Chart::getChartYMD(ymdhms);
    int hms = (int)fmod(ymdhms, 86400);

    return QDateTime(QDate(ymd / 10000, (ymd % 10000) / 100, ymd % 100),
        QTime(hms / 3600, (hms % 3600) / 60, hms % 60, ms));
}*/

FinanceDemo::FinanceDemo(QWidget *parent) :
        QDialog(parent, Qt::Window)
{
    cout << "FinanceDemo(Qwidget *parent) \n";    
    //
    // Initialize member variables
    //
    /*
    m_noOfPoints = 0;
    m_timeStamps = 0;
    m_highData = 0;
    m_lowData = 0;
    m_openData = 0;
    m_closeData = 0;
    m_volData = 0;

    m_resolution = 86400;

    m_compareData = 0;
    m_compareDataLen = 0;
    */
    //
    // Set up the GUI
    //

    setWindowTitle("Technical Analysis");
    resize(954, 520);
    QFont defaultFont("Arial");
    defaultFont.setPixelSize(11);
    setFont(defaultFont);
    setPalette(QPalette(QColor(0, 0, 0xff)));

    // Use horizontal layout to put the tree view on the left and the charts on the right
    QHBoxLayout *horizontalLayout = new QHBoxLayout(this);
    horizontalLayout->setMargin(0);
    horizontalLayout->setSpacing(0);

    // The frame on the left side
    QWidget *leftPanel = new QWidget(this);
    leftPanel->setAutoFillBackground(true);
    leftPanel->setPalette(QPalette(QColor(186, 221, 255)));
    leftPanel->setFixedWidth(156);
    horizontalLayout->addWidget(leftPanel);

    QFrame *separator = new QFrame(this);
    separator->setFrameStyle(QFrame::VLine | QFrame::Plain);
    horizontalLayout->addWidget(separator);

    QWidget *rightPanel = new QWidget(this);
    horizontalLayout->addWidget(rightPanel);
    rightPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Ticker Symbol
    (new QLabel("Ticker Symbol", leftPanel))->setGeometry(8, 10, 140, 18);
    m_TickerSymbol = new QLineEdit("AAPL", leftPanel);
    m_TickerSymbol->setGeometry(8, 26, 140, 20);

    // Compare With
    (new QLabel("Compare With", leftPanel))->setGeometry(8, 52, 140, 18);
    m_CompareWith = new QLineEdit(leftPanel);
    m_CompareWith->setGeometry(8, 68, 140, 20);

    // Time Period
    (new QLabel("Time Period", leftPanel))->setGeometry(8, 94, 140, 18);
    m_TimeRange = new QComboBox(leftPanel);
    m_TimeRange->setGeometry(8, 110, 140, 20);

    // Chart Size
    (new QLabel("Chart Size", leftPanel))->setGeometry(8, 136, 140, 18);
    m_ChartSize = new QComboBox(leftPanel);
    m_ChartSize->setGeometry(8, 152, 140, 20);

    // Value bars/Log Scale/Grid Lines
    m_VolumeBars = new QCheckBox("Show Volume Bars", leftPanel);
    m_ParabolicSAR = new QCheckBox("Parabolic SAR", leftPanel);
    m_LogScale = new QCheckBox("Log Scale", leftPanel);
    m_PercentageScale = new QCheckBox("Percentage Grid", leftPanel);
    m_VolumeBars->setChecked(true);
    m_VolumeBars->setGeometry(8, 176, 140, 20);
    m_ParabolicSAR->setGeometry(8, 196, 140, 20);
    m_LogScale->setGeometry(8, 216, 140, 20);
    m_PercentageScale->setGeometry(8, 236, 140, 20);

    // Chart Type
    (new QLabel("Chart Type", leftPanel))->setGeometry(8, 262, 140, 18);
    m_ChartType = new QComboBox(leftPanel);
    m_ChartType->setGeometry(8, 278, 140, 20);

    // Price Bands
    (new QLabel("Price Bands", leftPanel))->setGeometry(8, 304, 140, 18);
    m_PriceBand = new QComboBox(leftPanel);
    m_PriceBand->setGeometry(8, 320, 140, 20);

    // Moving Averages
    (new QLabel("Moving Averages", leftPanel))->setGeometry(8, 346, 140, 18);
    m_AvgType1 = new QComboBox(leftPanel);
    m_AvgType2 = new QComboBox(leftPanel);
    m_MovAvg1 = new QLineEdit("10", leftPanel);
    m_MovAvg2 = new QLineEdit("25", leftPanel);
    m_AvgType1->setGeometry(8, 362, 105, 20);
    m_AvgType2->setGeometry(8, 383, 105, 20);
    m_MovAvg1->setGeometry(113, 362, 35, 20);
    m_MovAvg2->setGeometry(113, 383, 35, 20);

    // Technical Indicators
    (new QLabel("Technical Indicators", leftPanel))->setGeometry(8, 409, 140, 18);
    m_Indicator1 = new QComboBox(leftPanel);
    m_Indicator2 = new QComboBox(leftPanel);
    m_Indicator3 = new QComboBox(leftPanel);
    m_Indicator4 = new QComboBox(leftPanel);
    m_Indicator1->setGeometry(8, 425, 140, 20);
    m_Indicator2->setGeometry(8, 446, 140, 20);
    m_Indicator3->setGeometry(8, 467, 140, 20);
    m_Indicator4->setGeometry(8, 488, 140, 20);

    QFont labelFont(defaultFont);
    labelFont.setBold(true);
    const QObjectList& children = leftPanel->children();
    for (int i = 0; i < children.size(); ++i)
    {
        QLabel *label = qobject_cast<QLabel *>(children[i]);
        if (0 != label)
            label->setFont(labelFont);
    }

    // Chart Viewer
    m_ChartViewer = new QChartViewer(rightPanel);
    m_ChartViewer->move(8, 12);

    //
    // Fill the contents of the combo boxes
    //

    m_TimeRange->addItem("1 day", 1);
    m_TimeRange->addItem("2 days", 2);
    m_TimeRange->addItem("5 days", 5);
    m_TimeRange->addItem("10 days", 10);
    m_TimeRange->addItem("1 month", 30);
    m_TimeRange->addItem("2 months", 60);
    m_TimeRange->addItem("3 months", 90);
    m_TimeRange->addItem("6 months", 180);
    m_TimeRange->addItem("1 year", 360);
    m_TimeRange->addItem("2 years", 720);
    m_TimeRange->addItem("3 years", 1080);
    m_TimeRange->addItem("4 years", 1440);
    m_TimeRange->addItem("5 years", 1800);
    m_TimeRange->addItem("10 years", 3600);
    m_TimeRange->setCurrentIndex(7);

    m_ChartSize->addItem("Small", "S");
    m_ChartSize->addItem("Medium", "M");
    m_ChartSize->addItem("Large", "L");
    m_ChartSize->addItem("Huge", "H");
    m_ChartSize->setCurrentIndex(2);

    m_ChartType->addItem("None", "None");
    m_ChartType->addItem("CandleStick", "CandleStick");
    m_ChartType->addItem("Closing Price", "Close");
    m_ChartType->addItem("Median Price", "Median");
    m_ChartType->addItem("OHLC", "OHLC");
    m_ChartType->addItem("Typical Price", "TP");
    m_ChartType->addItem("Weighted Close", "WC");
    m_ChartType->setCurrentIndex(1);

    m_PriceBand->addItem("None", "None");
    m_PriceBand->addItem("Bollinger Band", "BB");
    m_PriceBand->addItem("Donchain Channel", "DC");
    m_PriceBand->addItem("Envelop (SMA 20 +/- 10%)", "Envelop");
    m_PriceBand->setCurrentIndex(1);

    m_AvgType1->addItem("None", "None");
    m_AvgType1->addItem("Simple", "SMA");
    m_AvgType1->addItem("Exponential", "EMA");
    m_AvgType1->addItem("Triangular", "TMA");
    m_AvgType1->addItem("Weighted", "WMA");
    m_AvgType1->setCurrentIndex(1);

    for (int i = 0; i < m_AvgType1->count(); ++i)
        m_AvgType2->addItem(m_AvgType1->itemText(i), m_AvgType1->itemData(i));
    m_AvgType2->setCurrentIndex(1);

    //m_Indicator1->addItem("None", "None");
    m_Indicator1->addItem("Accumulation/Distribution", "AccDist");
    m_Indicator1->addItem("Aroon Oscillator", "AroonOsc");
    m_Indicator1->addItem("Aroon Up/Down", "Aroon");
    m_Indicator1->addItem("Avg Directional Index", "ADX");
    m_Indicator1->addItem("Avg True Range", "ATR");
    m_Indicator1->addItem("Bollinger Band Width", "BBW");
    m_Indicator1->addItem("Chaikin Money Flow", "CMF");
    m_Indicator1->addItem("Chaikin Oscillator", "COscillator");
    m_Indicator1->addItem("Chaikin Volatility", "CVolatility");
    m_Indicator1->addItem("Close Location Value", "CLV");
    m_Indicator1->addItem("Commodity Channel Index", "CCI");
    m_Indicator1->addItem("Detrended Price Osc", "DPO");
    m_Indicator1->addItem("Donchian Channel Width", "DCW");
    m_Indicator1->addItem("Ease of Movement", "EMV");
    m_Indicator1->addItem("Fast Stochastic", "FStoch");
    m_Indicator1->addItem("MACD", "MACD");
    m_Indicator1->addItem("Mass Index", "MDX");
    m_Indicator1->addItem("Momentum", "Momentum");
    m_Indicator1->addItem("Money Flow Index", "MFI");
    m_Indicator1->addItem("Neg Volume Index", "NVI");
    m_Indicator1->addItem("On Balance Volume", "OBV");
    m_Indicator1->addItem("Performance", "Performance");
    m_Indicator1->addItem("% Price Oscillator", "PPO");
    m_Indicator1->addItem("% Volume Oscillator", "PVO");
    m_Indicator1->addItem("Pos Volume Index", "PVI");
    m_Indicator1->addItem("Price Volume Trend", "PVT");
    m_Indicator1->addItem("Rate of Change", "ROC");
    m_Indicator1->addItem("RSI", "RSI");
    m_Indicator1->addItem("Slow Stochastic", "SStoch");
    m_Indicator1->addItem("StochRSI", "StochRSI");
    m_Indicator1->addItem("TRIX", "TRIX");
    m_Indicator1->addItem("Ultimate Oscillator", "UO");
    m_Indicator1->addItem("Volume", "Vol");
    m_Indicator1->addItem("William's %R", "WilliamR");

    for (int i = 0; i < m_Indicator1->count(); ++i)
    {
        m_Indicator2->addItem(m_Indicator1->itemText(i), m_Indicator1->itemData(i));
        m_Indicator3->addItem(m_Indicator1->itemText(i), m_Indicator1->itemData(i));
        m_Indicator4->addItem(m_Indicator1->itemText(i), m_Indicator1->itemData(i));
    }

    m_Indicator1->setCurrentIndex(m_Indicator1->findData("RSI"));
    m_Indicator2->setCurrentIndex(m_Indicator2->findData("MACD"));
    m_Indicator3->setCurrentIndex(m_Indicator2->findData("CCI"));
    m_Indicator4->setCurrentIndex(m_Indicator2->findData("StochRSI"));
    
    // Horizontal scroll bar
    m_HScrollBar = new QScrollBar(Qt::Horizontal, this);
    m_HScrollBar->setGeometry(165, 728, 1135, 17);
    connect(m_HScrollBar, SIGNAL(valueChanged(int)), SLOT(onHScrollBarChanged(int)));
    
    // Pointer push button
    QPushButton *pointerPB = new QPushButton(QIcon(":/pointer.png"), "Pointer");
    
    // The Pointer/Zoom In/Zoom Out buttons form a button group
    QButtonGroup *mouseUsage = new QButtonGroup(separator);
    mouseUsage->addButton(pointerPB, Chart::MouseUsageScroll);
    connect(mouseUsage, SIGNAL(buttonPressed(int)), SLOT(onMouseUsageChanged(int)));
    
    // Use the same action hanlder for all controls

    const QObjectList &allControls = leftPanel->children();
    for (int i = 0; i < allControls.count(); ++i)
    {
        QObject *obj;
        if ((obj = qobject_cast<QComboBox *>(allControls[i])) != 0)
            connect(obj, SIGNAL(currentIndexChanged(int)), SLOT(onComboBoxChanged(int)));
        else if ((obj = qobject_cast<QCheckBox *>(allControls[i])) != 0)
            connect(obj, SIGNAL(clicked()), SLOT(onCheckBoxChanged()));
        else if ((obj = qobject_cast<QLineEdit *>(allControls[i])) != 0)
            connect(obj, SIGNAL(editingFinished()), SLOT(onLineEditChanged()));
        else if ((obj = qobject_cast<QScrollBar *>(allControls[i])) != 0)
            connect(obj, SIGNAL(valueChanged(int)), SLOT(onHScrollBarChanged(int)));
        else if ((obj = qobject_cast<QButtonGroup *>(allControls[i])) != 0)
            connect(obj, SIGNAL(buttonPressed(int)), SLOT(onMouseUsageChanged(int)));
    }
    connect(m_ChartViewer, SIGNAL(viewPortChanged()), SLOT(onViewPortChanged()));
    connect(m_ChartViewer, SIGNAL(mouseMovePlotArea(QMouseEvent*)), SLOT(onMouseMovePlotArea(QMouseEvent*)));
    connect(m_ChartViewer, SIGNAL(mouseWheel(QWheelEvent*)), SLOT(onMouseWheelChart(QWheelEvent*)));
    
    // Load the data    
    read_data();   

    // Update the chart
    drawChart(m_ChartViewer);

    //
    // Initialize the chart
    //
        
    // Initialize the QChartViewer
    initChartViewer(m_ChartViewer);

    // Initially set the mouse to drag to scroll mode
    pointerPB->click();

    // Trigger the ViewPortChanged event to draw the chart
    m_ChartViewer->updateViewPort(true, true);
}

//
// Destructor
//
FinanceDemo::~FinanceDemo()
{
    cout << "~FinanceDemo \n";
    delete m_ChartViewer->getChart();
}

//
// Initialize the QChartViewer
//
void FinanceDemo::initChartViewer(QChartViewer *viewer)
{
    cout << "InitChartViewer \n";
    // Set the full x range to be the duration of the data
    viewer->setFullRange("x", date[0], date[data_len]);

    // Initialize the view port to show the latest 20% of the time range
    viewer->setViewPortWidth(0.04);
    viewer->setViewPortLeft(1 - viewer->getViewPortWidth());

    // Set the maximum zoom to 10 points
    viewer->setZoomInWidthLimit(50.0 / data_len);
}

void FinanceDemo::onCheckBoxChanged()
{
    cout << "onCheckBoxChanged \n";    
    drawChart(m_ChartViewer);
}

void FinanceDemo::onComboBoxChanged(int)
{
    cout << "onComboBoxChanged \n";
    drawChart(m_ChartViewer);
}

void FinanceDemo::onLineEditChanged()
{
    cout << "onLineEditChanged \n";

    int new_avgPeriod1 = m_MovAvg1->text().toInt();
    int new_avgPeriod2 = m_MovAvg2->text().toInt();

    QString tickerKey = m_TickerSymbol->text();
    
    QString compareKey = m_CompareWith->text();

    if (m_tickerKey != tickerKey) 
        read_data();

    if (m_compareKey != compareKey)
        read_data_compare();

    if ((new_avgPeriod1 != m_avgPeriod1) || (new_avgPeriod2 != m_avgPeriod2) ||
        (m_tickerKey != tickerKey) || (m_compareKey != compareKey))
    {
         m_tickerKey = m_TickerSymbol->text();
         m_compareKey =  m_CompareWith ->text();
         drawChart(m_ChartViewer);
    }
        
}

//
// User clicks on the the horizontal scroll bar
//
void FinanceDemo::onHScrollBarChanged(int value)
{
    cout << "onHScrollBarChanged \n";
    delete m_ChartViewer->getChart();
    if (!m_ChartViewer->isInViewPortChangedEvent())
    {
        // Set the view port based on the scroll bar
        int scrollBarLen = m_HScrollBar->maximum() + m_HScrollBar->pageStep();
        m_ChartViewer->setViewPortLeft(value / (double)scrollBarLen);

        // Update the chart display without updating the image maps. (We can delay updating
        // the image map until scrolling is completed and the chart display is stable.)
        m_ChartViewer->updateViewPort(true, false);
    }
}

//
// Update controls in the user interface when the view port changed
//
void FinanceDemo::updateControls(QChartViewer *viewer)
{
    cout << "updateControls \n";
    // The logical length of the scrollbar. It can be any large value. The actual value does
    // not matter.
    const int scrollBarLen = 1000000000;

    // Update the horizontal scroll bar
    m_HScrollBar->setEnabled(viewer->getViewPortWidth() < 1);
    m_HScrollBar->setPageStep((int)ceil(viewer->getViewPortWidth() * scrollBarLen));
    m_HScrollBar->setSingleStep(min(scrollBarLen / 100, m_HScrollBar->pageStep()));
    m_HScrollBar->setRange(0, scrollBarLen - m_HScrollBar->pageStep());
    m_HScrollBar->setValue((int)(0.5 + viewer->getViewPortLeft() * scrollBarLen));

}

//
// Draw track cursor when mouse is moving over plotarea
//
void FinanceDemo::onMouseMovePlotArea(QMouseEvent *)
{
    cout << "onMouseMovePlotArea \n";
    financedemo((MultiChart *)m_ChartViewer->getChart(), m_ChartViewer->getPlotAreaMouseX());
    m_ChartViewer->updateDisplay();
}

//
// The Pointer, Zoom In or Zoom out button is pressed
//
void FinanceDemo::onMouseUsageChanged(int mouseUsage)
{
    cout << "onMouseUsageChanged \n";
    m_ChartViewer->setMouseUsage(mouseUsage);
}

//
// The ViewPortChanged event handler. This event occurs if the user scrolls or zooms in
// or out the chart by dragging or clicking on the chart. It can also be triggered by
// calling WinChartViewer.updateViewPort.
//

void FinanceDemo::onViewPortChanged()
{
    cout << "onViewPortChanged \n";
	updateControls(m_ChartViewer);
    
    if (m_ChartViewer->needUpdateChart())
        drawChart(m_ChartViewer);
    // We need to update the track line too. If the mouse is moving on the chart (eg. if
    // the user drags the mouse on the chart to scroll it), the track line will be updated
    // in the MouseMovePlotArea event. Otherwise, we need to update the track line here.
    if (m_ChartViewer->needUpdateImageMap())
        updateImageMap(m_ChartViewer);
}

void FinanceDemo::updateImageMap(QChartViewer *viewer)
{
    cout << "updateImageMap \n";
    // Include tool tip for the chart
    if (0 == viewer->getImageMapHandler())
    {
        viewer->setImageMap(viewer->getChart()->getHTMLImageMap("", "",
            "title='[{dataSetName}] {x|mmm dd, yyyy}: USD {value|2}'"));
    }
}

//
// When the mouse enters the chart, we will generate an image map for hot spots and tooltips
// support if it has not already been generated.
//
void FinanceDemo::onMouseWheelChart(QWheelEvent *event)
{
    cout << "onMouseWheelChart \n";
    // Process the mouse wheel only if the mouse is over the plot area
    if (!m_ChartViewer->isMouseOnPlotArea())
    {
        event->ignore();
        return;
    }

    // We zoom in or out by 10% depending on the mouse wheel direction.
    double newVpWidth = m_ChartViewer->getViewPortWidth() * (event->delta() > 0 ? 0.9 : 1 / 0.9);
    double newVpHeight = m_ChartViewer->getViewPortHeight() * (event->delta() > 0 ? 0.9 : 1 / 0.9);

    // We do not zoom beyond the zoom width or height limits.
    newVpWidth = max(m_ChartViewer->getZoomInWidthLimit(), min(newVpWidth,
        m_ChartViewer->getZoomOutWidthLimit()));
    newVpHeight = max(m_ChartViewer->getZoomInHeightLimit(), min(newVpWidth,
        m_ChartViewer->getZoomOutHeightLimit()));

    if ((newVpWidth != m_ChartViewer->getViewPortWidth()) ||
            (newVpHeight != m_ChartViewer->getViewPortHeight()))
    {
        // Set the view port position and size so that the point under the mouse remains under
        // the mouse after zooming.

        double deltaX = (m_ChartViewer->getPlotAreaMouseX() - m_ChartViewer->getPlotAreaLeft()) *
            (m_ChartViewer->getViewPortWidth() - newVpWidth) / m_ChartViewer->getPlotAreaWidth();
        m_ChartViewer->setViewPortLeft(m_ChartViewer->getViewPortLeft() + deltaX);
        m_ChartViewer->setViewPortWidth(newVpWidth);

        double deltaY = (m_ChartViewer->getPlotAreaMouseY() - m_ChartViewer->getPlotAreaTop()) *
            (m_ChartViewer->getViewPortHeight() - newVpHeight) / m_ChartViewer->getPlotAreaHeight();
        m_ChartViewer->setViewPortTop(m_ChartViewer->getViewPortTop() + deltaY);
        m_ChartViewer->setViewPortHeight(newVpHeight);

        m_ChartViewer->updateViewPort(true, false);
    }
}


//
// Draw finance chart track line with legend
//
void FinanceDemo::financedemo(MultiChart *m, int mouseX)
{
    cout << "financedemo: \n";
    //cout << "mouseX" << mouseX << "\n";
    // Clear the current dynamic layer and get the DrawArea object to draw on it.
    DrawArea *d = m->initDynamicLayer();

    // It is possible for a FinanceChart to be empty, so we need to check for it.
    if (m->getChartCount() == 0)
        return ;
	//cout << "Chart count: " << m->getChartCount() << "\n" ;
    // Get the data x-value that is nearest to the mouse
    int xValue = (int)(((XYChart *)m->getChart(0))->getNearestXValue(mouseX));//layer
    //cout << "(XYChart *)m->getChart(0): " << (XYChart *)m->getChart(0) << "\n";
    //cout << "xValue: " << xValue << "\n";
	//cout << "xValue: " << xValue << "\n";
    // Iterate the XY charts (main price chart and indicator charts) in the FinanceChart
    XYChart *c = 0;
    for(int i = 0; i < m->getChartCount(); ++i) 
    {
        //cout << "IIIIiiii" << i << "\n";
        c = (XYChart *)m->getChart(i);
        //cout << "XYCHART: " << c << "\n";

        // Variables to hold the legend entries
        ostringstream ohlcLegend;
        vector<string> legendEntries;

        // Iterate through all layers to find the highest data point
        for(int j = 0; j < c->getLayerCount(); ++j) 
        {
            //cout << "JJJJjjjj" << j << "\n";
            //cout << "layer count: " << c->getLayerCount() << "\n";
            Layer *layer = c->getLayerByZ(j);
            //cout << "Layer: " << layer << "\n";
            int xIndex = layer->getXIndexOf(xValue);
            //cout << "xIndex: " << xIndex << "\n";
            int dataSetCount = layer->getDataSetCount();
            //cout << "data set count: " << dataSetCount << "\n";

            // In a FinanceChart, only layers showing OHLC data can have 4 data sets
            if (dataSetCount == 4) 
            {
                double highValue = layer->getDataSet(0)->getValue(xIndex);
                //cout << "high: " << highValue << "\n";
                double lowValue = layer->getDataSet(1)->getValue(xIndex);
                //cout << "low: " << lowValue << "\n";
                double openValue = layer->getDataSet(2)->getValue(xIndex);
                //cout << "open: " << openValue << "\n";
                double closeValue = layer->getDataSet(3)->getValue(xIndex);
                //cout << "close: " << closeValue << "\n";
				
				//cout << "Chart noValue: " << Chart::NoValue << "\n";
                if (closeValue != Chart::NoValue) 
                {
                    // Build the OHLC legend
					ohlcLegend << "      <*block*>";
					ohlcLegend << "Open: " << c->formatValue(openValue, "{value|P4}");
					ohlcLegend << ", High: " << c->formatValue(highValue, "{value|P4}"); 
					ohlcLegend << ", Low: " << c->formatValue(lowValue, "{value|P4}"); 
					ohlcLegend << ", Close: " << c->formatValue(closeValue, "{value|P4}");
					//cout << "Display open: " << c->formatValue(openValue, "{value|P4}") << "\n";
					//cout << "Display high: " << c->formatValue(highValue, "{value|P4}") << "\n";
					//cout << "Display low: " << c->formatValue(lowValue, "{value|P4}") << "\n";
					//cout << "Display close: " << c->formatValue(closeValue, "{value|P4}") << "\n";
					
                    // We also draw an upward or downward triangle for up and down days and the %
                    // change
                    double lastCloseValue = layer->getDataSet(3)->getValue(xIndex - 1);
                    //cout << "Last Close Value: " << lastCloseValue << "\n";
                    if (lastCloseValue != Chart::NoValue) 
                    {
                        double change = closeValue - lastCloseValue;
                        //double percent = change * 100 / closeValue;
                        //cout << "Percent" << percent << "\n";
                        string symbol = (change >= 0) ?
                            "<*font,color=008800*><*img=@triangle,width=8,color=008800*>" :
                            "<*font,color=CC0000*><*img=@invertedtriangle,width=8,color=CC0000*>";

                        ohlcLegend << "  " << symbol << " " << c->formatValue(change, "{value|P4}");
                    }

					ohlcLegend << "<*/*>";
                }
            } 
            else 
            {
                // Iterate through all the data sets in the layer
                for(int k = 0; k < layer->getDataSetCount(); ++k)
                {
                    DataSet *dataSet = layer->getDataSetByZ(k);
					//cout << "Dataset by Z: " << dataset << "\n";
                    string name = dataSet->getDataName();
                    //cout << "Dataset Name: " << name << "\n";
                    double value = dataSet->getValue(xIndex);
                    //cout << "Data set Value: " << value << "\n";
                    //cout << "name size: " << name.size() << "\n";
                    if ((0 != name.size()) && (value != Chart::NoValue)) 
                    {
                        // In a FinanceChart, the data set name consists of the indicator name and its
                        // latest value. It is like "Vol: 123M" or "RSI (14): 55.34". As we are
                        // generating the values dynamically, we need to extract the indictor name
                        // out, and also the volume unit (if any).

						// The volume unit
						string unitChar;
						//cout << "unit char: " << unitChar << "\n";

                        // The indicator name is the part of the name up to the colon character.
						int delimiterPosition = (int)name.find(':');
						//cout << "delimiter position " << delimiterPosition << "\n";
						//cout << "name nPOS: " << name.npos << "\n";
                        if ((int)name.npos != delimiterPosition) 
                        {							
							// The unit, if any, is the trailing non-digit character(s).
							int lastDigitPos = (int)name.find_last_of("0123456789");
							//cout << "Last digit position: " << lastDigitPos << "\n";
                            if (((int)name.npos != lastDigitPos) && (lastDigitPos + 1 < (int)name.size())
                                && (lastDigitPos > delimiterPosition))
								unitChar = name.substr(lastDigitPos + 1);
								//cout << "unit char 2: " << unitChar << "\n";

							name.resize(delimiterPosition);
                        }

                        // In a FinanceChart, if there are two data sets, it must be representing a
                        // range.
                        if (dataSetCount == 2) 
                        {
                            // We show both values in the range in a single legend entry
                            value = layer->getDataSet(0)->getValue(xIndex);
                            //cout << "value22: " << value << "\n";
                            double value2 = layer->getDataSet(1)->getValue(xIndex);
                            //cout << "value2: " << value2 << "\n";
                            name = name + ": " + c->formatValue(min(value, value2), "{value|P3}");
							name = name + " - " + c->formatValue(max(value, value2), "{value|P3}");
                        } 
                        else 
                        {
                            // In a FinanceChart, only the layer for volume bars has 3 data sets for
                            // up/down/flat days
                            if (dataSetCount == 3) 
                            {
                                // The actual volume is the sum of the 3 data sets.
                                value = layer->getDataSet(0)->getValue(xIndex) + layer->getDataSet(1
                                    )->getValue(xIndex) + layer->getDataSet(2)->getValue(xIndex);
                                //cout << "value in if1: " << value << "\n";
                                //cout << "layer->getDataSet(0)->getValue(xIndex): " << layer->getDataSet(0)->getValue(xIndex) << "\n";
                                //cout << "layer->getDataSet(1)->getValue(xIndex): " << layer->getDataSet(1)->getValue(xIndex) << "\n";
                                //cout << "layer->getDataSet(2)->getValue(xIndex): " << layer->getDataSet(1)->getValue(xIndex) << "\n";                                    
                            }

                            // Create the legend entry
                            name = name + ": " + c->formatValue(value, "{value|P3}") + unitChar;
                            //cout << "name legend entry: " << name << "\n";
                        }

                        // Build the legend entry, consist of a colored square box and the name (with
                        // the data value in it).
						ostringstream legendEntry;
						legendEntry << "<*block*><*img=@square,width=8,edgeColor=000000,color=" 
							<< hex << dataSet->getDataColor() << "*> " << name << "<*/*>";
						//cout << "legend Entry: " << legendEntry << "\n";	
                        legendEntries.push_back(legendEntry.str());
                    }
                }
            }
        }

        // Get the plot area position relative to the entire FinanceChart
        PlotArea *plotArea = c->getPlotArea();
        //cout << "plot area: " << plotArea << "\n";
        int plotAreaLeftX = plotArea->getLeftX() + c->getAbsOffsetX();
        //cout << "plot area leftx: " << plotAreaLeftX << "\n";
        int plotAreaTopY = plotArea->getTopY() + c->getAbsOffsetY();
        //cout << "plot area topy: " << plotAreaTopY << "\n";

		// The legend begins with the date label, then the ohlcLegend (if any), and then the
		// entries for the indicators.
		ostringstream legendText;
		legendText << "<*block,valign=top,maxWidth=" << (plotArea->getWidth() - 5) 
			<< "*><*font=arialbd.ttf*>[" << c->xAxis()->getFormattedLabel(xValue, "mmm dd, yyyy")
			<< "]<*/font*>" << ohlcLegend.str();
		for (int i = ((int)legendEntries.size()) - 1; i >= 0; --i)
		{
			legendText << "      " << legendEntries[i];
			//cout << "legend text: " << legendText << "\n";
		}
		legendText << "<*/*>";

        // Draw a vertical track line at the x-position
        d->vline(plotAreaTopY, plotAreaTopY + plotArea->getHeight(), c->getXCoor(xValue) +
            c->getAbsOffsetX(), d->dashLineColor(0x000000, 0x0101));

        // Display the legend on the top of the plot area
        TTFText *t = d->text(legendText.str().c_str(), "arial.ttf", 8);
        t->draw(plotAreaLeftX + 5, plotAreaTopY + 3, 0x000000, Chart::TopLeft);
		t->destroy();
    }
}

/////////////////////////////////////////////////////////////////////////////
// Data source

/// <summary>
/// Get the timeStamps, highData, lowData, openData, closeData and volData.
/// </summary>
/// <param name="ticker">The ticker symbol for the data series.</param>
/// <param name="startDate">The starting date/time for the data series.</param>
/// <param name="endDate">The ending date/time for the data series.</param>
/// <param name="durationInDays">The number of trading days to get.</param>
/// <param name="extraPoints">The extra leading data points needed in order to
/// compute moving averages.</param>
/// <returns>True if successfully obtain the data, otherwise false.</returns>
/*bool FinanceDemo::getData(const QString &ticker, QDateTime startDate, QDateTime endDate,
                          int durationInDays, int extraPoints)
{
    // This method should return false if the ticker symbol is invalid. In this
    // sample code, as we are using a random number generator for the data, all
    // ticker symbol is allowed, but we still assumed an empty symbol is invalid.
    if (ticker.isEmpty())
        return false;

    // In this demo, we can get 15 min, daily, weekly or monthly data depending on
    // the time range.
    m_resolution = 86400;
    if (durationInDays <= 10)
    {
        // 10 days or less, we assume 15 minute data points are available
        m_resolution = 900;

        // We need to adjust the startDate backwards for the extraPoints. We assume
        // 6.5 hours trading time per day, and 5 trading days per week.
        double dataPointsPerDay = 6.5 * 3600 / m_resolution;
        QDateTime adjustedStartDate(startDate.date().addDays(
                -(int)(extraPoints / dataPointsPerDay * 7 / 5 + 2.9999999)));

        // Get the required 15 min data
        get15MinData(ticker, adjustedStartDate, endDate);
    }
    else if (durationInDays >= 4.5 * 360)
    {
        // 4 years or more - use monthly data points.
        m_resolution = 30 * 86400;

        // Adjust startDate backwards to cater for extraPoints
        QDateTime adjustedStartDate(startDate.date().addMonths(-extraPoints));

        // Get the required monthly data
        getMonthlyData(ticker, adjustedStartDate, endDate);
    }
    else if (durationInDays >= 1.5 * 360)
    {
        // 1 year or more - use weekly points.
        m_resolution = 7 * 86400;

        //Note that we need to add extra points by shifting the starting weeks backwards
        QDateTime adjustedStartDate(startDate.date().addDays(-extraPoints * 7 - 6));

        // Get the required weekly data
        getWeeklyData(ticker, adjustedStartDate, endDate);
    }
    else
    {
        // Default - use daily points
        m_resolution = 86400;

        // Adjust startDate backwards to cater for extraPoints. We multiply the days
        // by 7/5 as we assume 1 week has 5 trading days.
        QDateTime adjustedStartDate(startDate.date().addDays(-((extraPoints * 7 + 4) / 5 + 2)));

        // Get the required daily data
        getDailyData(ticker, adjustedStartDate, endDate);
    }

    return true;
}

/// <summary>
/// Get 15 minutes data series into the timeStamps, highData, lowData, openData, closeData
/// and volData arrays.
/// </summary>
/// <param name="startDate">The starting date/time for the data series.</param>
/// <param name="endDate">The ending date/time for the data series.</param>
void FinanceDemo::get15MinData(const QString &ticker, QDateTime startDate, QDateTime endDate)
{
    //
    // In this demo, we use a random number generator to generate the data. In practice,
    // you may get the data from a database or by other means. If you do not have 15
    // minute data, you may modify the "drawChart" method below to not using 15 minute
    // data.
    //
    generateRandomData(ticker, startDate, endDate, 900);
}

/// <summary>
/// Get daily data series into the timeStamps, highData, lowData, openData, closeData and
/// volData arrays.
/// </summary>
/// <param name="startDate">The starting date/time for the data series.</param>
/// <param name="endDate">The ending date/time for the data series.</param>
void FinanceDemo::getDailyData(const QString &ticker, QDateTime startDate, QDateTime endDate)
{
    //
    // In this demo, we use a random number generator to generate the data. In practice,
    // you may get the data from a database or by other means. Replace the code below
    // with your own data acquisition code.
    //

    generateRandomData(ticker, startDate, endDate, 86400);
}

/// <summary>
/// Get weekly data series into the timeStamps, highData, lowData, openData, closeData and
/// volData arrays.
/// </summary>
/// <param name="startDate">The starting date/time for the data series.</param>
/// <param name="endDate">The ending date/time for the data series.</param>
void FinanceDemo::getWeeklyData(const QString &ticker, QDateTime startDate, QDateTime endDate)
{
    //
    // In this demo, we use a random number generator to generate the data. In practice,
    // you may get the data from a database or by other means. If you do not have weekly
    // data, you may call "getDailyData" to get daily data first, and then call
    // "convertDailyToWeeklyData" to convert it to weekly data, like:
    //
    //      getDailyData(ticker, startDate, endDate);
    //      convertDailyToWeeklyData();
    //
    generateRandomData(ticker, startDate, endDate, 86400 * 7);
}

/// <summary>
/// Get monthly data series into the timeStamps, highData, lowData, openData, closeData and
/// volData arrays.
/// </summary>
/// <param name="startDate">The starting date/time for the data series.</param>
/// <param name="endDate">The ending date/time for the data series.</param>
void FinanceDemo::getMonthlyData(const QString &ticker, QDateTime startDate, QDateTime endDate)
{
    //
    // In this demo, we use a random number generator to generate the data. In practice,
    // you may get the data from a database or by other means. If you do not have monthly
    // data, you may call "getDailyData" to get daily data first, and then call
    // "convertDailyToMonthlyData" to convert it to monthly data, like:
    //
    //      getDailyData(ticker, startDate, endDate);
    //      convertDailyToMonthlyData();
    //
    generateRandomData(ticker, startDate, endDate, 86400 * 30);
}

/// <summary>
/// A random number generator designed to generate realistic financial data.
/// </summary>
/// <param name="startDate">The starting date/time for the data series.</param>
/// <param name="endDate">The ending date/time for the data series.</param>
/// <param name="resolution">The period of the data series.</param>
void FinanceDemo::generateRandomData(const QString &ticker, QDateTime startDate,
                                     QDateTime endDate, int resolution)
{
    // free the previous data arrays
    delete[] m_timeStamps;
    delete[] m_highData;
    delete[] m_lowData;
    delete[] m_openData;
    delete[] m_closeData;
    delete[] m_volData;

    // The financial simulator
    FinanceSimulator db(ticker.toUtf8().data(), QDateTimeToChartTime(startDate),
                        QDateTimeToChartTime(endDate), resolution);

    // Allocate the data arrays
    m_noOfPoints = db.getTimeStamps().len;
    m_timeStamps = new double[m_noOfPoints];
    m_highData = new double[m_noOfPoints];
    m_lowData = new double[m_noOfPoints];
    m_openData = new double[m_noOfPoints];
    m_closeData = new double[m_noOfPoints];
    m_volData = new double[m_noOfPoints];

    // Copy data to the data arrays
    memcpy(m_timeStamps, db.getTimeStamps().data, m_noOfPoints * sizeof(double));
    memcpy(m_highData, db.getHighData().data, m_noOfPoints * sizeof(double));
    memcpy(m_lowData, db.getLowData().data, m_noOfPoints * sizeof(double));
    memcpy(m_openData, db.getOpenData().data, m_noOfPoints * sizeof(double));
    memcpy(m_closeData, db.getCloseData().data, m_noOfPoints * sizeof(double));
    memcpy(m_volData, db.getVolData().data, m_noOfPoints * sizeof(double));
}

/// <summary>
/// A utility to convert daily to weekly data.
/// </summary>
void FinanceDemo::convertDailyToWeeklyData()
{
    aggregateData(ArrayMath(DoubleArray(m_timeStamps, m_noOfPoints)).selectStartOfWeek());
}

/// <summary>
/// A utility to convert daily to monthly data.
/// </summary>
void FinanceDemo::convertDailyToMonthlyData()
{
    aggregateData(ArrayMath(DoubleArray(m_timeStamps, m_noOfPoints)).selectStartOfMonth());
}

/// <summary>
/// An internal method used to aggregate daily data.
/// </summary>
void FinanceDemo::aggregateData(ArrayMath &aggregator)
{
    aggregator.aggregate(DoubleArray(m_highData, m_noOfPoints), Chart::AggregateMax);
    aggregator.aggregate(DoubleArray(m_lowData, m_noOfPoints), Chart::AggregateMin);
    aggregator.aggregate(DoubleArray(m_openData, m_noOfPoints), Chart::AggregateFirst);
    aggregator.aggregate(DoubleArray(m_closeData, m_noOfPoints), Chart::AggregateLast);
    aggregator.aggregate(DoubleArray(m_volData, m_noOfPoints), Chart::AggregateSum);
    m_noOfPoints = aggregator.aggregate(DoubleArray(m_timeStamps, m_noOfPoints),
        Chart::AggregateFirst).len;
}*/

/////////////////////////////////////////////////////////////////////////////
// Chart Creation

/// <summary>
/// Add a moving average line to the FinanceChart object.
/// </summary>
/// <param name="m">The FinanceChart object to add the line to.</param>
/// <param name="avgType">The moving average type (SMA/EMA/TMA/WMA).</param>
/// <param name="avgPeriod">The moving average period.</param>
/// <param name="color">The color of the line.</param>
static LineLayer* addMovingAvg(FinanceChart *m, QString avgType, int avgPeriod, int color)
{
    cout << "addMovingAvg \n";
    if (avgPeriod > 1)
    {
        if (avgType == "SMA")
            return m->addSimpleMovingAvg(avgPeriod, color);
        else if (avgType == "EMA")
            return m->addExpMovingAvg(avgPeriod, color);
        else if (avgType == "TMA")
            return m->addTriMovingAvg(avgPeriod, color);
        else if (avgType == "WMA")
            return m->addWeightedMovingAvg(avgPeriod, color);
    }

    return 0;
}

/// <summary>
/// Add an indicator chart to the FinanceChart object. In this demo example, the indicator
/// parameters (such as the period used to compute RSI, colors of the lines, etc.) are hard
/// coded to commonly used values. You are welcome to design a more complex user interface
/// to allow users to set the parameters.
/// </summary>
/// <param name="m">The FinanceChart object to add the line to.</param>
/// <param name="indicator">The selected indicator.</param>
/// <param name="height">Height of the chart in pixels</param>
static XYChart* addIndicator(FinanceChart *m, QString indicator, int height)
{
    cout << "addIndicator \n";
    if (indicator == "RSI")
        return m->addRSI(height, 14, 0x800080, 20, 0xff6666, 0x6666ff);
    else if (indicator == "StochRSI")
        return m->addStochRSI(height, 14, 0x800080, 30, 0xff6666, 0x6666ff);
    else if (indicator == "MACD")
        return m->addMACD(height, 26, 12, 9, 0xff, 0xff00ff, 0x8000);
    else if (indicator == "FStoch")
        return m->addFastStochastic(height, 14, 3, 0x6060, 0x606000);
    else if (indicator == "SStoch")
        return m->addSlowStochastic(height, 14, 3, 0x6060, 0x606000);
    else if (indicator == "ATR")
        return m->addATR(height, 14, 0x808080, 0xff);
    else if (indicator == "ADX")
        return m->addADX(height, 14, 0x8000, 0x800000, 0x80);
    else if (indicator == "DCW")
        return m->addDonchianWidth(height, 20, 0xff);
    else if (indicator == "BBW")
        return m->addBollingerWidth(height, 20, 2, 0xff);
    else if (indicator == "DPO")
        return m->addDPO(height, 20, 0xff);
    else if (indicator == "PVT")
        return m->addPVT(height, 0xff);
    else if (indicator == "Momentum")
        return m->addMomentum(height, 12, 0xff);
    else if (indicator == "Performance")
        return m->addPerformance(height, 0xff);
    else if (indicator == "ROC")
        return m->addROC(height, 12, 0xff);
    else if (indicator == "OBV")
        return m->addOBV(height, 0xff);
    else if (indicator == "AccDist")
        return m->addAccDist(height, 0xff);
    else if (indicator == "CLV")
        return m->addCLV(height, 0xff);
    else if (indicator == "WilliamR")
        return m->addWilliamR(height, 14, 0x800080, 30, 0xff6666, 0x6666ff);
    else if (indicator == "Aroon")
        return m->addAroon(height, 14, 0x339933, 0x333399);
    else if (indicator == "AroonOsc")
        return m->addAroonOsc(height, 14, 0xff);
    else if (indicator == "CCI")
        return m->addCCI(height, 20, 0x800080, 100, 0xff6666, 0x6666ff);
    else if (indicator == "EMV")
        return m->addEaseOfMovement(height, 9, 0x6060, 0x606000);
    else if (indicator == "MDX")
        return m->addMassIndex(height, 0x800080, 0xff6666, 0x6666ff);
    else if (indicator == "CVolatility")
        return m->addChaikinVolatility(height, 10, 10, 0xff);
    else if (indicator == "COscillator")
        return m->addChaikinOscillator(height, 0xff);
    else if (indicator == "CMF")
        return m->addChaikinMoneyFlow(height, 21, 0x8000);
    else if (indicator == "NVI")
        return m->addNVI(height, 255, 0xff, 0x883333);
    else if (indicator == "PVI")
        return m->addPVI(height, 255, 0xff, 0x883333);
    else if (indicator == "MFI")
        return m->addMFI(height, 14, 0x800080, 30, 0xff6666, 0x6666ff);
    else if (indicator == "PVO")
        return m->addPVO(height, 26, 12, 9, 0xff, 0xff00ff, 0x8000);
    else if (indicator == "PPO")
        return m->addPPO(height, 26, 12, 9, 0xff, 0xff00ff, 0x8000);
    else if (indicator == "UO")
        return m->addUltimateOscillator(height, 7, 14, 28, 0x800080, 20, 0xff6666, 0x6666ff);
    else if (indicator == "Vol")
        return m->addVolIndicator(height, 0x99ff99, 0xff9999, 0xc0c0c0);
    else if (indicator == "TRIX")
        return m->addTRIX(height, 12, 0xff);
    else
        return 0;
}

/// <summary>
/// Creates a dummy chart to show an error message.
/// </summary>
/// <param name="viewer">The WinChartViewer to display the error message.</param>
/// <param name="msg">The error message</param>
/*static void errMsg(QChartViewer* viewer, const char *msg)
{
    cout << "errMsg \n";
    MultiChart m(400, 200);
    m.addTitle(Chart::Center, msg, "Arial", 10)->setMaxWidth(m.getWidth());
    viewer->setChart(&m);
}*/

/// <summary>
/// Draw the chart according to user selection and display it in the ChartViewer.
/// </summary>
/// <param name="viewer">The ChartViewer object to display the chart.</param>
void FinanceDemo::drawChart(QChartViewer *viewer)
{
   cout << "drawChart \n";
    /*// In this demo, we just assume we plot up to the latest time. So endDate is now.
    QDateTime endDate = QDateTime::currentDateTime();

    // If the trading day has not yet started (before 9:30am), or if the end date is on
    // on Sat or Sun, we set the end date to 4:00pm of the last trading day
    while ((endDate.time() < QTime(9, 30, 0)) || (endDate.date().dayOfWeek() >= 6)) {
        endDate = QDateTime(endDate.date().addDays(-1), QTime(16, 0, 0));
    }

     // The duration selected by the user
    int durationInDays = m_TimeRange->itemData(m_TimeRange->currentIndex()).toInt();

    // Compute the start date by subtracting the duration from the end date.
    QDateTime startDate = endDate;
    if (durationInDays >= 30) {
        // More or equal to 30 days - so we use months as the unit
        startDate = QDateTime(QDate(endDate.date().year(), endDate.date().month(), 1
                                    ).addMonths(-durationInDays / 30));
    } else {
        // Less than 30 days - use day as the unit. The starting point of the axis is
        // always at the start of the day (9:30am). Note that we use trading days, so
        // we skip Sat and Sun in counting the days.
        startDate = QDateTime(endDate.date());
        for(int i = 1; i < durationInDays; ++i) {
            if (startDate.date().dayOfWeek() == 1) {
                startDate = startDate.addDays(-3);
            } else {
                startDate = startDate.addDays(-1);
            }
        }
    }*/   
        
    //read_data();
   
    // Get the start date and end date that are visible on the chart.
    double viewPortStartDate = viewer->getValueAtViewPort("x", viewer->getViewPortLeft());
    double viewPortEndDate = viewer->getValueAtViewPort("x", viewer->getViewPortLeft() +
        viewer->getViewPortWidth());

    // Get the array indexes that corresponds to the visible start and end dates
    int startIndex = (int)floor(Chart::bSearch(DoubleArray(date, data_len), viewPortStartDate));
    cout << "Start Index: " << startIndex << "\n";
    int endIndex = (int)ceil(Chart::bSearch(DoubleArray(date, data_len), viewPortEndDate));
    int noOfPoints = endIndex - startIndex + 1;        

    // The first moving average period selected by the user.
    m_avgPeriod1 = m_MovAvg1->text().toInt();
    if (m_avgPeriod1 < 0)
        m_avgPeriod1 = 0;
    if (m_avgPeriod1 > 300)
        m_avgPeriod1 = 300;

    // The second moving average period selected by the user.
    m_avgPeriod2 = m_MovAvg2->text().toInt();
    if (m_avgPeriod2 < 0)
        m_avgPeriod2 = 0;
    if (m_avgPeriod2 > 300)
        m_avgPeriod2 = 300;

    // We need extra leading data points in order to compute moving averages.
    int extraPoints = (m_avgPeriod1 > m_avgPeriod2) ? m_avgPeriod1 : m_avgPeriod2;
    if (extraPoints < 25)
        extraPoints = 25;

    // Get the data series to compare with, if any.
    /*m_compareKey = m_CompareWith->text();
    delete[] m_compareData;
    m_compareData = 0;
    if (getData(m_compareKey, startDate, endDate, durationInDays, extraPoints))
    {
            m_compareData = m_closeData;
            m_compareDataLen = m_noOfPoints;
            m_closeData = 0;
    }*/

    // The data series we want to get.
    /*m_tickerKey = m_TickerSymbol->text();
    if (!getData(m_tickerKey, startDate, endDate, durationInDays, extraPoints))
    {
            errMsg(viewer, "Please enter a valid ticker symbol");
            return;
    }

    // We now confirm the actual number of extra points (data points that are before
    // the start date) as inferred using actual data from the database.
    double startChartTime = QDateTimeToChartTime(startDate);
    for (extraPoints = 0; extraPoints < m_noOfPoints; ++extraPoints)
    {
        if (m_timeStamps[extraPoints] >= startChartTime)
            break;
    }

    // Check if there is any valid data
    if (extraPoints >= m_noOfPoints)
    {
        // No data - just display the no data message.
        errMsg(viewer, "No data available for the specified time period");
        return;
    }

    // In some finance chart presentation style, even if the data for the latest day
    // is not fully available, the axis for the entire day will still be drawn, where
    // no data will appear near the end of the axis.
    int extraTrailingPoints = 0;
    if (m_resolution <= 86400)
    {
        // Add extra points to the axis until it reaches the end of the day. The end
        // of day is assumed to be 16:00 (it depends on the stock exchange).
        double lastTime = m_timeStamps[m_noOfPoints - 1];
        extraTrailingPoints = (int)((16 * 3600 - fmod(lastTime, 86400)) / m_resolution);
        if (extraTrailingPoints > 0)
        {
            double *extendedTimeStamps = new double[m_noOfPoints + extraTrailingPoints];
            memcpy(extendedTimeStamps, m_timeStamps, sizeof(double) * m_noOfPoints);
            for (int i = 0; i < extraTrailingPoints; ++i)
                extendedTimeStamps[m_noOfPoints + i] = lastTime + m_resolution * (i + 1);
            delete[] m_timeStamps;
            m_timeStamps = extendedTimeStamps;
        }
    }*/

    //
    // At this stage, all data is available. We can draw the chart as according to
    // user input.
    //

    //
    // Determine the chart size. In this demo, user can select 4 different chart sizes.
    // Default is the large chart size.
    //
    int width = 1135;
    int mainHeight = 255;
    int indicatorHeight = 100;

    QString selectedSize = m_ChartSize->itemData(m_ChartSize->currentIndex()).toString();
    if (selectedSize == "S")
    {
        // Small chart size
        width = 450;
        mainHeight = 160;
        indicatorHeight = 60;
    }
    else if (selectedSize == "M")
    {
        // Medium chart size
        width = 620;
        mainHeight = 215;
        indicatorHeight = 70;
    }
    else if (selectedSize == "H")
    {
        // Huge chart size
        width = 1000;
        mainHeight = 320;
        indicatorHeight = 90;
    }

    // Create the chart object using the selected size
    FinanceChart *m = new FinanceChart(width);

    // Add a title to the chart
    if(data_len <= 0)
        m->addTitle("INVALID SYMBOL");    
    else
        m->addTitle(Chart::TopCenter, m_TickerSymbol->text().toLocal8Bit().data());

    // Disable default legend box, as we are using dynamic legend
    m->setLegendStyle("normal", 8, Chart::Transparent, Chart::Transparent);
    
    // Set the data into the finance chart object
    m->setData(DoubleArray(date + startIndex, noOfPoints), 
               DoubleArray(high + startIndex, noOfPoints), 
               DoubleArray(low + startIndex, noOfPoints), 
               DoubleArray(open + startIndex, noOfPoints), 
               DoubleArray(close + startIndex, noOfPoints), 
               DoubleArray(volume + startIndex, noOfPoints), 30);

    /*// Set the data into the chart object
    m.setData(DoubleArray(m_timeStamps, m_noOfPoints + extraTrailingPoints),
        DoubleArray(m_highData, m_noOfPoints), DoubleArray(m_lowData, m_noOfPoints),
        DoubleArray(m_openData, m_noOfPoints), DoubleArray(m_closeData, m_noOfPoints),
        DoubleArray(m_volData, m_noOfPoints), extraPoints);

    //
    // We configure the title of the chart. In this demo chart design, we put the
    // company name as the top line of the title with left alignment.
    //
    m.addPlotAreaTitle(Chart::TopLeft, m_tickerKey.toUtf8().data());

    // We displays the current date as well as the data resolution on the next line.
    const char *resolutionText = "";
    if (m_resolution == 30 * 86400)
        resolutionText = "Monthly";
    else if (m_resolution == 7 * 86400)
        resolutionText = "Weekly";
    else if (m_resolution == 86400)
        resolutionText = "Daily";
    else if (m_resolution == 900)
        resolutionText = "15-min";

    char buffer[1024];
    sprintf(buffer, "<*font=arial.ttf,size=8*>%s - %s chart", m.formatValue(
        QDateTimeToChartTime(QDateTime::currentDateTime()), "mmm dd, yyyy"), resolutionText);
    m.addPlotAreaTitle(Chart::BottomLeft, buffer);
    */
    // A copyright message at the bottom left corner the title area
    m->addPlotAreaTitle(Chart::BottomRight,
        "<*font=arial.ttf,size=8*>(c) Advanced Software Engineering");
    
    //
    // Add the first techical indicator according. In this demo, we draw the first
    // indicator on top of the main chart.
    //
    addIndicator(m, m_Indicator1->itemData(m_Indicator1->currentIndex()).toString(),
                 indicatorHeight);

        //
    // Add the main chart
        //
    m->addMainChart(mainHeight);

    //
    // Set log or linear scale according to user preference
    //
    m->setLogScale(m_LogScale->isChecked());

        //
        // Set axis labels to show data values or percentage change to user preference
        //
        if (m_PercentageScale->isChecked())
                m->setPercentageAxis();

        //
    // Draw the main chart depending on the chart type the user has selected
    //
    QString selectedType = m_ChartType->itemData(m_ChartType->currentIndex()).toString();
    if (selectedType == "Close")
        m->addCloseLine(0x000040);
    else if (selectedType == "TP")
        m->addTypicalPrice(0x000040);
    else if (selectedType == "WC")
        m->addWeightedClose(0x000040);
    else if (selectedType == "Median")
        m->addMedianPrice(0x000040);

    //
    // Add comparison line if there is data for comparison
    //
    if (m_compareDataLen > 30) {
        cout << "for comparison \n";    
            m->addComparison(DoubleArray(m_compareData + startIndex, noOfPoints), 0x0000ff,
                m_compareKey.toUtf8().data());
    }
 

    //
    // Add moving average lines.
    //
    addMovingAvg(m, m_AvgType1->itemData(m_AvgType1->currentIndex()).toString(),
                 m_avgPeriod1, 0x663300);
    addMovingAvg(m, m_AvgType2->itemData(m_AvgType2->currentIndex()).toString(),
                 m_avgPeriod2, 0x9900ff);

    //
    // Draw the main chart if the user has selected CandleStick or OHLC. We
    // draw it here to make sure it is drawn behind the moving average lines
    // (that is, the moving average lines stay on top.)
    //
    if (selectedType == "CandleStick")
        m->addCandleStick(0x33ff33, 0xff3333);
    else if (selectedType == "OHLC")
        m->addHLOC(0x8000, 0x800000);

    //
    // Add parabolic SAR if necessary
    //
    if (m_ParabolicSAR->isChecked())
        m->addParabolicSAR(0.02, 0.02, 0.2, Chart::DiamondShape, 5, 0x008800, 0x000000);

    //
    // Add price band/channel/envelop to the chart according to user selection
    //
    QString selectedBand = m_PriceBand->itemData(m_PriceBand->currentIndex()).toString();
    if (selectedBand == "BB")
        m->addBollingerBand(20, 2, 0x9999ff, 0xc06666ff);
    else if (selectedBand == "DC")
        m->addDonchianChannel(20, 0x9999ff, 0xc06666ff);
    else if (selectedBand == "Envelop")
        m->addEnvelop(20, 0.1, 0x9999ff, 0xc06666ff);

    //
    // Add volume bars to the main chart if necessary
    //
    if (m_VolumeBars->isChecked())
        m->addVolBars(indicatorHeight, 0x99ff99, 0xff9999, 0xc0c0c0);

    //
    // Add additional indicators as according to user selection.
    //
    addIndicator(m, m_Indicator2->itemData(m_Indicator2->currentIndex()).toString(),
                 indicatorHeight);
    addIndicator(m, m_Indicator3->itemData(m_Indicator3->currentIndex()).toString(),
                 indicatorHeight);
    addIndicator(m, m_Indicator4->itemData(m_Indicator4->currentIndex()).toString(),
                 indicatorHeight);

    // Include track line with legend for the latest data values
    financedemo(m, ((XYChart *)m->getChart(0))->getPlotArea()->getRightX());

    // Set the chart to the viewer
    viewer->setChart(m);

    /*// Set image map (for tool tips) to the viewer
    sprintf(buffer, "title='%s {value|P}'", m.getToolTipDateFormat());
    viewer->setImageMap(m.getHTMLImageMap("", "", buffer));*/
}

void FinanceDemo :: read_data()
{
    cout << "read_data \n";
    struct tm tm1;
    try {
        Connection conn(false);
        conn.connect("technical_analysis", "localhost", "root", "waheguru13");        
        Query query = conn.query();     
       
        char *s = m_TickerSymbol->text().toLocal8Bit().data();
        
        /* Now SELECT */        
        query << "SELECT id FROM stocks where stock_name = " << quote_only << s;
        StoreQueryResult cres = query.store();        
        if (cres.num_rows() == 0)
        {
            data_len = 0;
            return;
        }          
        
        int stock_id = cres[0]["id"];        
        query << "SELECT * FROM stocks_details where stock_id = " << stock_id ;
        StoreQueryResult ares = query.store();
        data_len = ares.num_rows() - 1;

        date = new double[data_len + 1];
        open = new double[data_len + 1];
        high = new double[data_len + 1];
        low = new double[data_len + 1];
        close = new double[data_len + 1];
        volume = new double[data_len + 1];
        for (size_t i = 0; i < ares.num_rows(); i++)
        {            
            sscanf(ares[i]["date"].c_str(),"%4d-%2d-%2d",&tm1.tm_year,&tm1.tm_mon,&tm1.tm_mday);
            date[i] = Chart::chartTime(tm1.tm_year , tm1.tm_mon, tm1.tm_mday);
            open[i] = ares[i]["open"];
            high[i] = ares[i]["high"];
            low[i] = ares[i]["low"];
            close[i] = ares[i]["close"];
            volume[i] = ares[i]["volume"];                        
        }        
    } catch (BadQuery er) { // handle any connection or
        // query errors that may come up
        cout << "BAD QUERY 1" << "\n";
        cerr << "Error: " << er.what() << endl;        
    } catch (const BadConversion& er) {
        // Handle bad conversions
        cout << "BAD QUERY 2" << "\n";
        cerr << "Conversion error: " << er.what() << endl <<
                "\tretrieved data size: " << er.retrieved <<
                ", actual size: " << er.actual_size << endl;        
    } catch (const Exception& er) {
        // Catch-all for any other MySQL++ exceptions
        cout << "BAD QUERY 3" << "\n";
        cerr << "Error: " << er.what() << endl;        
    } 

}

void FinanceDemo :: read_data_compare()
{
    cout << "read_data_compare \n";
    
    try {
        Connection conn(false);
        conn.connect("technical_analysis", "localhost", "root", "waheguru13");        
        Query query = conn.query();         
        char *s =  m_CompareWith ->text().toLocal8Bit().data();
        
        /* Now SELECT */        
        query << "SELECT id FROM stocks where stock_name = " << quote_only << s;
        StoreQueryResult cres = query.store();        
        if (cres.num_rows() == 0)
        {
           m_compareDataLen = 0;
            return;
        }          
        
        int stock_id = cres[0]["id"];        
        query << "SELECT * FROM stocks_details where stock_id = " << stock_id ;
        StoreQueryResult ares = query.store();
        m_compareDataLen = ares.num_rows() - 1;

        m_compareData = new double[m_compareDataLen + 1];

        for (size_t i = 0; i < ares.num_rows(); i++)
        {            
            m_compareData[i] = ares[i]["close"];            
        }        
    } catch (BadQuery er) { // handle any connection or
        // query errors that may come up
        cout << "BAD QUERY 1" << "\n";
        cerr << "Error: " << er.what() << endl;        
    } catch (const BadConversion& er) {
        // Handle bad conversions
        cout << "BAD QUERY 2" << "\n";
        cerr << "Conversion error: " << er.what() << endl <<
                "\tretrieved data size: " << er.retrieved <<
                ", actual size: " << er.actual_size << endl;        
    } catch (const Exception& er) {
        // Catch-all for any other MySQL++ exceptions
        cout << "BAD QUERY 3" << "\n";
        cerr << "Error: " << er.what() << endl;        
    } 
}


