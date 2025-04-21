#include "standalone_dashboard.h"
#include <QDateTime>
#include <QStringList>
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include <QPalette>
#include <QFont>
#include <QStyleFactory>
#include <QMessageBox>
#include <QStatusBar> // Add this include for statusBar() access

StandaloneDashboard::StandaloneDashboard(QWidget *parent)
    : QMainWindow(parent)
    , uiUpdateTimer(new QTimer(this))
{
    setWindowTitle("ERA - Efficient Resource Allocation Dashboard");
    
    // Initialize resource components
    cpuMonitor = std::make_unique<CPUMonitor>();
    memMonitor = std::make_unique<MemoryMonitor>();
    powerMonitor = std::make_unique<PowerMonitor>();
    cpuOptimizer = std::make_unique<CPUOptimizer>();
    memOptimizer = std::make_unique<MemoryOptimizer>();
    powerOptimizer = std::make_unique<PowerOptimizer>();
    population = std::make_unique<Population>(20); // 20 chromosomes in population
    
    // Set up UI
    setupUI();
    
    // Initialize timer for UI updates
    connect(uiUpdateTimer, &QTimer::timeout, this, &StandaloneDashboard::updateUI);
    uiUpdateTimer->start(1000); // Update UI every second
    
    // Start the optimization thread
    startOptimizationThread();
    
    // Set minimum size for the window
    resize(1000, 800);
}

StandaloneDashboard::~StandaloneDashboard()
{
    // Stop optimization thread
    runOptimization = false;
    
    if (optimizationThreadObj.joinable()) {
        optimizationThreadObj.join();
    }
    
    uiUpdateTimer->stop();
}

void StandaloneDashboard::setupUI()
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
    statusBar()->showMessage("Started standalone ERA optimization and monitoring");
}

void StandaloneDashboard::setupResourceMonitors()
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

void StandaloneDashboard::setupLoadControls()
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
    connect(lightLoadButton, &QPushButton::clicked, this, &StandaloneDashboard::setLightLoad);
    connect(mediumLoadButton, &QPushButton::clicked, this, &StandaloneDashboard::setMediumLoad);
    connect(spikeLoadButton, &QPushButton::clicked, this, &StandaloneDashboard::setSpikeLoad);
    
    // Default to light load
    lightLoadButton->setChecked(true);
    
    loadLayout->addStretch();
    loadLayout->addWidget(lightLoadButton);
    loadLayout->addWidget(mediumLoadButton);
    loadLayout->addWidget(spikeLoadButton);
    loadLayout->addStretch();
    
    mainLayout->addWidget(loadGroupBox);
}

void StandaloneDashboard::setupCharts()
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

void StandaloneDashboard::startOptimizationThread()
{
    optimizationThreadObj = std::thread(&StandaloneDashboard::optimizationThread, this);
}

void StandaloneDashboard::optimizationThread()
{
    auto lastTime = std::chrono::steady_clock::now();
    
    while (runOptimization) {
        try {
            // Get current resource usage
            {
                std::lock_guard<std::mutex> lock(resourceMutex);
                currentResources.cpu_usage = cpuMonitor->getCurrentUsage();
                currentResources.memory_usage = memMonitor->getCurrentUsage();
                currentResources.power_usage = powerMonitor->getCurrentUsage();
            }
            
            // Run genetic algorithm optimization
            population->evolve();
            Chromosome bestChromosome = population->getBestChromosome();
            
            // Apply optimizations
            {
                std::lock_guard<std::mutex> lock(resourceMutex);
                optimizedParams = bestChromosome.getParams();
                cpuOptimizer->optimize(optimizedParams.cpu_threshold);
                memOptimizer->optimize(optimizedParams.memory_threshold);
                powerOptimizer->optimize(optimizedParams.power_threshold);
            }
            
            // Record history data every second
            auto currentTime = std::chrono::steady_clock::now();
            auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();
            
            if (elapsedMs >= 1000) { // 1 second interval
                lastTime = currentTime;
                
                HistoryPoint point;
                point.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
                    currentTime.time_since_epoch()).count();
                
                {
                    std::lock_guard<std::mutex> lock(resourceMutex);
                    point.cpu_usage = currentResources.cpu_usage;
                    point.memory_usage = currentResources.memory_usage;
                    point.power_usage = currentResources.power_usage;
                    point.cpu_threshold = optimizedParams.cpu_threshold;
                    point.memory_threshold = optimizedParams.memory_threshold;
                    point.power_threshold = optimizedParams.power_threshold;
                    point.fitness = bestChromosome.getFitness();
                }
                
                {
                    std::lock_guard<std::mutex> lock(historyMutex);
                    historyData.append(point);
                    
                    // Limit history size
                    if (historyData.size() > MAX_HISTORY_POINTS) {
                        historyData.removeFirst();
                    }
                }
            }
        } catch (const std::exception& e) {
            // Log error but continue
            qWarning("Error in optimization thread: %s", e.what());
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void StandaloneDashboard::updateUI()
{
    // Update UI with latest data
    updateResourceMonitors();
    updateCharts();
    
    statusBar()->showMessage(QString("ERA optimization running | Last update: %1")
                           .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
}

void StandaloneDashboard::updateResourceMonitors()
{
    std::lock_guard<std::mutex> lock(resourceMutex);
    
    // Update CPU
    cpuLabel->setText(QString("CPU: %1%").arg(currentResources.cpu_usage, 0, 'f', 1));
    cpuProgressBar->setValue(currentResources.cpu_usage);
    cpuThresholdLabel->setText(QString("Threshold: %1%").arg(optimizedParams.cpu_threshold, 0, 'f', 1));
    
    // Color code the progress bar based on usage vs threshold
    if (currentResources.cpu_usage > optimizedParams.cpu_threshold) {
        cpuProgressBar->setStyleSheet("QProgressBar::chunk { background-color: #F44336; }");
    } else {
        cpuProgressBar->setStyleSheet("QProgressBar::chunk { background-color: #4CAF50; }");
    }
    
    // Update Memory
    memoryLabel->setText(QString("Memory: %1%").arg(currentResources.memory_usage, 0, 'f', 1));
    memoryProgressBar->setValue(currentResources.memory_usage);
    memoryThresholdLabel->setText(QString("Threshold: %1%").arg(optimizedParams.memory_threshold, 0, 'f', 1));
    
    // Color code the progress bar based on usage vs threshold
    if (currentResources.memory_usage > optimizedParams.memory_threshold) {
        memoryProgressBar->setStyleSheet("QProgressBar::chunk { background-color: #F44336; }");
    } else {
        memoryProgressBar->setStyleSheet("QProgressBar::chunk { background-color: #4CAF50; }");
    }
    
    // Update Power
    powerLabel->setText(QString("Power: %1W").arg(currentResources.power_usage, 0, 'f', 2));
    powerProgressBar->setValue(currentResources.power_usage * 100); // Scale to int range
    powerThresholdLabel->setText(QString("Threshold: %1W").arg(optimizedParams.power_threshold, 0, 'f', 2));
    
    // Color code the progress bar based on usage vs threshold
    if (currentResources.power_usage > optimizedParams.power_threshold) {
        powerProgressBar->setStyleSheet("QProgressBar::chunk { background-color: #F44336; }");
    } else {
        powerProgressBar->setStyleSheet("QProgressBar::chunk { background-color: #4CAF50; }");
    }
    
    // Update load level buttons
    QAbstractButton* button = loadButtonGroup->button(currentLoadLevel.load());
    if (button && !button->isChecked()) {
        button->setChecked(true);
    }
}

void StandaloneDashboard::updateCharts()
{
    std::lock_guard<std::mutex> lock(historyMutex);
    
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

void StandaloneDashboard::setLoadLevel(int level)
{
    if (level >= 0 && level <= 2) {
        currentLoadLevel.store(level);
        
        // Update the status message
        QString loadText;
        switch (level) {
            case 0: loadText = "Light"; break;
            case 1: loadText = "Medium"; break;
            case 2: loadText = "Spike"; break;
            default: loadText = "Unknown";
        }
        
        statusBar()->showMessage(QString("Changed load level to %1").arg(loadText));
    }
}

void StandaloneDashboard::setLightLoad()
{
    setLoadLevel(0);
}

void StandaloneDashboard::setMediumLoad()
{
    setLoadLevel(1);
}

void StandaloneDashboard::setSpikeLoad()
{
    setLoadLevel(2);
}