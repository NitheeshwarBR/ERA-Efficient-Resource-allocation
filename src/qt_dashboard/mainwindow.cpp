#include "mainwindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QDateTime>
#include <QStringList>
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include <QPalette>
#include <QFont>
#include <QStyleFactory>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , networkManager(new QNetworkAccessManager(this))
    , dataTimer(new QTimer(this))
    , historyTimer(new QTimer(this))
{
    setWindowTitle("ERA - Efficient Resource Allocation Dashboard");
    
    // Set up UI
    setupUI();
    
    // Initialize connection to API
    connect(dataTimer, &QTimer::timeout, this, &MainWindow::updateData);
    connect(historyTimer, &QTimer::timeout, this, &MainWindow::fetchHistory);
    
    // Start timers
    dataTimer->start(1000); // Update every second
    historyTimer->start(5000); // Fetch history every 5 seconds
    
    // Initial data update
    updateData();
    fetchHistory();
    
    // Set minimum size for the window
    resize(1000, 800);
}

MainWindow::~MainWindow()
{
    dataTimer->stop();
    historyTimer->stop();
}

void MainWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    mainLayout = new QVBoxLayout(centralWidget);
    
    // Title label
    QLabel* titleLabel = new QLabel("ERA - Efficient Resource Allocation", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    QLabel* subtitleLabel = new QLabel("Real-time monitoring and optimization using genetic algorithms", this);
    subtitleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(subtitleLabel);
    
    // Setup components
    setupResourceMonitors();
    setupLoadControls();
    setupCharts();
    
    // Status bar
    statusBar()->showMessage("Connecting to ERA server...");
}

void MainWindow::setupResourceMonitors()
{
    resourceGroupBox = new QGroupBox("Current Resources", this);
    resourceLayout = new QVBoxLayout(resourceGroupBox);
    
    // CPU
    QHBoxLayout* cpuLayout = new QHBoxLayout();
    cpuLabel = new QLabel("CPU: 0%", this);
    cpuProgressBar = new QProgressBar(this);
    cpuProgressBar->setRange(0, 100);
    cpuProgressBar->setValue(0);
    cpuThresholdLabel = new QLabel("Threshold: 0%", this);
    
    cpuLayout->addWidget(cpuLabel, 2);
    cpuLayout->addWidget(cpuProgressBar, 6);
    cpuLayout->addWidget(cpuThresholdLabel, 2);
    resourceLayout->addLayout(cpuLayout);
    
    // Memory
    QHBoxLayout* memoryLayout = new QHBoxLayout();
    memoryLabel = new QLabel("Memory: 0%", this);
    memoryProgressBar = new QProgressBar(this);
    memoryProgressBar->setRange(0, 100);
    memoryProgressBar->setValue(0);
    memoryThresholdLabel = new QLabel("Threshold: 0%", this);
    
    memoryLayout->addWidget(memoryLabel, 2);
    memoryLayout->addWidget(memoryProgressBar, 6);
    memoryLayout->addWidget(memoryThresholdLabel, 2);
    resourceLayout->addLayout(memoryLayout);
    
    // Power
    QHBoxLayout* powerLayout = new QHBoxLayout();
    powerLabel = new QLabel("Power: 0W", this);
    powerProgressBar = new QProgressBar(this);
    powerProgressBar->setRange(0, 15 * 100); // 0 to 15W, scaled by 100
    powerProgressBar->setValue(0);
    powerThresholdLabel = new QLabel("Threshold: 0W", this);
    
    powerLayout->addWidget(powerLabel, 2);
    powerLayout->addWidget(powerProgressBar, 6);
    powerLayout->addWidget(powerThresholdLabel, 2);
    resourceLayout->addLayout(powerLayout);
    
    mainLayout->addWidget(resourceGroupBox);
}

void MainWindow::setupLoadControls()
{
    loadGroupBox = new QGroupBox("Load Control", this);
    loadLayout = new QHBoxLayout(loadGroupBox);
    
    lightLoadButton = new QPushButton("Light Load", this);
    mediumLoadButton = new QPushButton("Medium Load", this);
    spikeLoadButton = new QPushButton("Spike Load", this);
    
    // Style buttons
    lightLoadButton->setStyleSheet("QPushButton:checked { background-color: #4CAF50; color: white; }");
    mediumLoadButton->setStyleSheet("QPushButton:checked { background-color: #FF9800; color: white; }");
    spikeLoadButton->setStyleSheet("QPushButton:checked { background-color: #F44336; color: white; }");
    
    // Make buttons checkable
    lightLoadButton->setCheckable(true);
    mediumLoadButton->setCheckable(true);
    spikeLoadButton->setCheckable(true);
    
    // Create button group
    loadButtonGroup = new QButtonGroup(this);
    loadButtonGroup->addButton(lightLoadButton, 0);
    loadButtonGroup->addButton(mediumLoadButton, 1);
    loadButtonGroup->addButton(spikeLoadButton, 2);
    
    // Connect signals
    connect(lightLoadButton, &QPushButton::clicked, this, &MainWindow::setLightLoad);
    connect(mediumLoadButton, &QPushButton::clicked, this, &MainWindow::setMediumLoad);
    connect(spikeLoadButton, &QPushButton::clicked, this, &MainWindow::setSpikeLoad);
    
    // Default to light load
    lightLoadButton->setChecked(true);
    
    loadLayout->addStretch();
    loadLayout->addWidget(lightLoadButton);
    loadLayout->addWidget(mediumLoadButton);
    loadLayout->addWidget(spikeLoadButton);
    loadLayout->addStretch();
    
    mainLayout->addWidget(loadGroupBox);
}

void MainWindow::setupCharts()
{
    chartsGroupBox = new QGroupBox("Resource Monitoring", this);
    chartsLayout = new QVBoxLayout(chartsGroupBox);
    
    // Resource usage chart
    resourceChart = new QChart();
    resourceChart->setTitle("Resource Usage Over Time");
    
    cpuSeries = new QLineSeries();
    cpuSeries->setName("CPU Usage (%)");
    
    cpuThresholdSeries = new QLineSeries();
    cpuThresholdSeries->setName("CPU Threshold (%)");
    cpuThresholdSeries->setPen(QPen(Qt::red, 1, Qt::DashLine));
    
    memorySeries = new QLineSeries();
    memorySeries->setName("Memory Usage (%)");
    
    memoryThresholdSeries = new QLineSeries();
    memoryThresholdSeries->setName("Memory Threshold (%)");
    memoryThresholdSeries->setPen(QPen(Qt::blue, 1, Qt::DashLine));
    
    powerSeries = new QLineSeries();
    powerSeries->setName("Power Usage (W)");
    
    powerThresholdSeries = new QLineSeries();
    powerThresholdSeries->setName("Power Threshold (W)");
    powerThresholdSeries->setPen(QPen(Qt::green, 1, Qt::DashLine));
    
    resourceChart->addSeries(cpuSeries);
    resourceChart->addSeries(cpuThresholdSeries);
    resourceChart->addSeries(memorySeries);
    resourceChart->addSeries(memoryThresholdSeries);
    resourceChart->addSeries(powerSeries);
    resourceChart->addSeries(powerThresholdSeries);
    
    // Create axes
    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("Time (s)");
    axisX->setLabelFormat("%i");
    resourceChart->addAxis(axisX, Qt::AlignBottom);
    
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Usage (%)");
    axisY->setRange(0, 100);
    resourceChart->addAxis(axisY, Qt::AlignLeft);
    
    QValueAxis *axisY2 = new QValueAxis();
    axisY2->setTitleText("Power (W)");
    axisY2->setRange(0, 15);
    resourceChart->addAxis(axisY2, Qt::AlignRight);
    
    // Attach axes to series
    cpuSeries->attachAxis(axisX);
    cpuSeries->attachAxis(axisY);
    
    cpuThresholdSeries->attachAxis(axisX);
    cpuThresholdSeries->attachAxis(axisY);
    
    memorySeries->attachAxis(axisX);
    memorySeries->attachAxis(axisY);
    
    memoryThresholdSeries->attachAxis(axisX);
    memoryThresholdSeries->attachAxis(axisY);
    
    powerSeries->attachAxis(axisX);
    powerSeries->attachAxis(axisY2);
    
    powerThresholdSeries->attachAxis(axisX);
    powerThresholdSeries->attachAxis(axisY2);
    
    resourceChartView = new QChartView(resourceChart);
    resourceChartView->setRenderHint(QPainter::Antialiasing);
    resourceChartView->setMinimumHeight(300);
    
    // Fitness chart
    fitnessChart = new QChart();
    fitnessChart->setTitle("Genetic Algorithm Fitness");
    
    fitnessSeries = new QLineSeries();
    fitnessSeries->setName("Fitness Score");
    
    fitnessChart->addSeries(fitnessSeries);
    
    // Create axes for fitness chart
    QValueAxis *fitnessAxisX = new QValueAxis();
    fitnessAxisX->setTitleText("Time (s)");
    fitnessAxisX->setLabelFormat("%i");
    fitnessChart->addAxis(fitnessAxisX, Qt::AlignBottom);
    
    QValueAxis *fitnessAxisY = new QValueAxis();
    fitnessAxisY->setTitleText("Fitness");
    fitnessAxisY->setRange(0, 1);
    fitnessChart->addAxis(fitnessAxisY, Qt::AlignLeft);
    
    fitnessSeries->attachAxis(fitnessAxisX);
    fitnessSeries->attachAxis(fitnessAxisY);
    
    fitnessChartView = new QChartView(fitnessChart);
    fitnessChartView->setRenderHint(QPainter::Antialiasing);
    fitnessChartView->setMinimumHeight(200);
    
    chartsLayout->addWidget(resourceChartView);
    chartsLayout->addWidget(fitnessChartView);
    
    mainLayout->addWidget(chartsGroupBox);
}

void MainWindow::updateData()
{
    QNetworkRequest request(QUrl(serverUrl + "/api/resources"));
    QNetworkReply* reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        onResourceDataReceived(reply);
    });
}

void MainWindow::fetchHistory()
{
    QNetworkRequest request(QUrl(serverUrl + "/api/history"));
    QNetworkReply* reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        onHistoryDataReceived(reply);
    });
}

void MainWindow::onResourceDataReceived(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        QJsonObject jsonObj = jsonDoc.object();
        
        // Update resource data structure
        QJsonObject current = jsonObj["current"].toObject();
        QJsonObject optimized = jsonObj["optimized"].toObject();
        
        currentData.cpu = current["cpu"].toDouble();
        currentData.memory = current["memory"].toDouble();
        currentData.power = current["power"].toDouble();
        currentData.cpu_threshold = optimized["cpu_threshold"].toDouble();
        currentData.memory_threshold = optimized["memory_threshold"].toDouble();
        currentData.power_threshold = optimized["power_threshold"].toDouble();
        currentData.load_level = jsonObj["load_level"].toInt();
        
        // Update UI
        updateResourceMonitors();
        
        // Update load level buttons
        QAbstractButton* button = loadButtonGroup->button(currentData.load_level);
        if (button && !button->isChecked()) {
            button->setChecked(true);
        }
        
        statusBar()->showMessage("Connected to ERA server | Last update: " + 
                               QDateTime::currentDateTime().toString("hh:mm:ss"));
    } else {
        statusBar()->showMessage("Error: Failed to connect to ERA server: " + reply->errorString());
    }
    
    reply->deleteLater();
}

void MainWindow::onHistoryDataReceived(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        QJsonArray jsonArray = jsonDoc.array();
        
        historyData.clear();
        
        for (const QJsonValue& value : jsonArray) {
            QJsonObject obj = value.toObject();
            HistoryPoint point;
            
            point.timestamp = obj["timestamp"].toDouble();
            point.cpu_usage = obj["cpu_usage"].toDouble();
            point.memory_usage = obj["memory_usage"].toDouble();
            point.power_usage = obj["power_usage"].toDouble();
            point.cpu_threshold = obj["cpu_threshold"].toDouble();
            point.memory_threshold = obj["memory_threshold"].toDouble();
            point.power_threshold = obj["power_threshold"].toDouble();
            point.fitness = obj["fitness"].toDouble();
            
            historyData.append(point);
        }
        
        updateCharts();
    }
    
    reply->deleteLater();
}

void MainWindow::updateResourceMonitors()
{
    // Update CPU
    cpuLabel->setText(QString("CPU: %1%").arg(currentData.cpu, 0, 'f', 1));
    cpuProgressBar->setValue(currentData.cpu);
    cpuThresholdLabel->setText(QString("Threshold: %1%").arg(currentData.cpu_threshold, 0, 'f', 1));
    
    // Color code the progress bar based on usage vs threshold
    if (currentData.cpu > currentData.cpu_threshold) {
        cpuProgressBar->setStyleSheet("QProgressBar::chunk { background-color: #F44336; }");
    } else {
        cpuProgressBar->setStyleSheet("QProgressBar::chunk { background-color: #4CAF50; }");
    }
    
    // Update Memory
    memoryLabel->setText(QString("Memory: %1%").arg(currentData.memory, 0, 'f', 1));
    memoryProgressBar->setValue(currentData.memory);
    memoryThresholdLabel->setText(QString("Threshold: %1%").arg(currentData.memory_threshold, 0, 'f', 1));
    
    // Color code the progress bar based on usage vs threshold
    if (currentData.memory > currentData.memory_threshold) {
        memoryProgressBar->setStyleSheet("QProgressBar::chunk { background-color: #F44336; }");
    } else {
        memoryProgressBar->setStyleSheet("QProgressBar::chunk { background-color: #4CAF50; }");
    }
    
    // Update Power
    powerLabel->setText(QString("Power: %1W").arg(currentData.power, 0, 'f', 2));
    powerProgressBar->setValue(currentData.power * 100); // Scale to int range
    powerThresholdLabel->setText(QString("Threshold: %1W").arg(currentData.power_threshold, 0, 'f', 2));
    
    // Color code the progress bar based on usage vs threshold
    if (currentData.power > currentData.power_threshold) {
        powerProgressBar->setStyleSheet("QProgressBar::chunk { background-color: #F44336; }");
    } else {
        powerProgressBar->setStyleSheet("QProgressBar::chunk { background-color: #4CAF50; }");
    }
}

void MainWindow::updateCharts()
{
    if (historyData.isEmpty()) {
        return;
    }
    
    // Clear existing data
    cpuSeries->clear();
    cpuThresholdSeries->clear();
    memorySeries->clear();
    memoryThresholdSeries->clear();
    powerSeries->clear();
    powerThresholdSeries->clear();
    fitnessSeries->clear();
    
    // Base timestamp for relative time (x-axis)
    double baseTimestamp = historyData.first().timestamp;
    
    // Add new data points
    for (const HistoryPoint& point : historyData) {
        double relativeTime = point.timestamp - baseTimestamp;
        
        cpuSeries->append(relativeTime, point.cpu_usage);
        cpuThresholdSeries->append(relativeTime, point.cpu_threshold);
        memorySeries->append(relativeTime, point.memory_usage);
        memoryThresholdSeries->append(relativeTime, point.memory_threshold);
        powerSeries->append(relativeTime, point.power_usage);
        powerThresholdSeries->append(relativeTime, point.power_threshold);
        fitnessSeries->append(relativeTime, point.fitness);
    }
    
    // Update chart axes
    if (historyData.size() > 1) {
        double duration = historyData.last().timestamp - baseTimestamp;
        
        // Resource chart X axis
        QValueAxis* axisX = qobject_cast<QValueAxis*>(resourceChart->axes(Qt::Horizontal).first());
        if (axisX) {
            axisX->setRange(0, duration);
        }
        
        // Fitness chart X axis
        QValueAxis* fitnessAxisX = qobject_cast<QValueAxis*>(fitnessChart->axes(Qt::Horizontal).first());
        if (fitnessAxisX) {
            fitnessAxisX->setRange(0, duration);
        }
    }
}

void MainWindow::setLoadLevel(int level)
{
    QNetworkRequest request(QUrl(QString("%1/api/load/%2").arg(serverUrl).arg(level)));
    networkManager->get(request);
    
    // Update the status message
    QString loadText;
    switch (level) {
        case 0: loadText = "Light"; break;
        case 1: loadText = "Medium"; break;
        case 2: loadText = "Spike"; break;
        default: loadText = "Unknown";
    }
    
    statusBar()->showMessage(QString("Changing load level to %1...").arg(loadText));
}

void MainWindow::setLightLoad()
{
    setLoadLevel(0);
}

void MainWindow::setMediumLoad()
{
    setLoadLevel(1);
}

void MainWindow::setSpikeLoad()
{
    setLoadLevel(2);
}
