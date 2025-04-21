#pragma once

#include <QMainWindow>
#include <QTimer>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QButtonGroup>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>
#include <QVector>
#include <QStatusBar> // Add this include
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <memory>

#include "resource/cpu/cpu_monitor.hpp"
#include "resource/memory/memory_monitor.hpp"
#include "resource/power/power_monitor.hpp"
#include "resource/cpu/cpu_optimizer.hpp"
#include "resource/memory/memory_optimizer.hpp"
#include "resource/power/power_optimizer.hpp"
#include "genetic/population.hpp"
#include "utils/data_types.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class StandaloneDashboard; }
QT_CHARTS_USE_NAMESPACE
QT_END_NAMESPACE

struct HistoryPoint {
    double timestamp;
    double cpu_usage;
    double memory_usage;
    double power_usage;
    double cpu_threshold;
    double memory_threshold;
    double power_threshold;
    double fitness;
};

class StandaloneDashboard : public QMainWindow
{
    Q_OBJECT

public:
    StandaloneDashboard(QWidget *parent = nullptr);
    ~StandaloneDashboard();

private slots:
    void updateUI();
    void setLoadLevel(int level);
    void setLightLoad();
    void setMediumLoad();
    void setSpikeLoad();

private:
    void setupUI();
    void setupResourceMonitors();
    void setupLoadControls();
    void setupCharts();
    void updateCharts();
    void updateResourceMonitors();
    void startOptimizationThread();
    void optimizationThread();

    // UI components
    QWidget* centralWidget;
    QVBoxLayout* mainLayout;
    
    // Resource monitors
    QGroupBox* resourceGroupBox;
    QVBoxLayout* resourceLayout;
    QLabel* cpuLabel;
    QProgressBar* cpuProgressBar;
    QLabel* cpuThresholdLabel;
    QLabel* memoryLabel;
    QProgressBar* memoryProgressBar;
    QLabel* memoryThresholdLabel;
    QLabel* powerLabel;
    QProgressBar* powerProgressBar;
    QLabel* powerThresholdLabel;

    // Load control
    QGroupBox* loadGroupBox;
    QHBoxLayout* loadLayout;
    QButtonGroup* loadButtonGroup;
    QPushButton* lightLoadButton;
    QPushButton* mediumLoadButton;
    QPushButton* spikeLoadButton;

    // Charts
    QGroupBox* chartsGroupBox;
    QVBoxLayout* chartsLayout;
    QChartView* resourceChartView;
    QChart* resourceChart;
    QLineSeries* cpuSeries;
    QLineSeries* cpuThresholdSeries;
    QLineSeries* memorySeries;
    QLineSeries* memoryThresholdSeries;
    QLineSeries* powerSeries;
    QLineSeries* powerThresholdSeries;
    
    QChartView* fitnessChartView;
    QChart* fitnessChart;
    QLineSeries* fitnessSeries;

    // Resource monitoring and optimization components
    std::unique_ptr<CPUMonitor> cpuMonitor;
    std::unique_ptr<MemoryMonitor> memMonitor;
    std::unique_ptr<PowerMonitor> powerMonitor;
    std::unique_ptr<CPUOptimizer> cpuOptimizer;
    std::unique_ptr<MemoryOptimizer> memOptimizer;
    std::unique_ptr<PowerOptimizer> powerOptimizer;
    std::unique_ptr<Population> population;
    
    // Data structures and synchronization
    SystemResources currentResources;
    OptimizationParams optimizedParams;
    std::mutex resourceMutex;
    std::atomic<int> currentLoadLevel{0}; // 0=light, 1=medium, 2=spike
    std::atomic<bool> runOptimization{true}; // Control optimization thread
    std::thread optimizationThreadObj;
    
    // History data for charts
    QVector<HistoryPoint> historyData;
    std::mutex historyMutex;
    const size_t MAX_HISTORY_POINTS = 120;
    
    // Timer for UI updates
    QTimer* uiUpdateTimer;
};
