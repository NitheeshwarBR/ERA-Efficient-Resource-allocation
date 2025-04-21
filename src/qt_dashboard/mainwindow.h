#pragma once

#include <QMainWindow>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
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

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_CHARTS_USE_NAMESPACE
QT_END_NAMESPACE

struct ResourceData {
    double cpu;
    double memory;
    double power;
    double cpu_threshold;
    double memory_threshold;
    double power_threshold;
    int load_level;
};

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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateData();
    void fetchHistory();
    void onResourceDataReceived(QNetworkReply* reply);
    void onHistoryDataReceived(QNetworkReply* reply);
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

    // Data and networking
    QNetworkAccessManager* networkManager;
    QTimer* dataTimer;
    QTimer* historyTimer;
    ResourceData currentData;
    QVector<HistoryPoint> historyData;

    const QString serverUrl = "http://localhost:8080";
};
