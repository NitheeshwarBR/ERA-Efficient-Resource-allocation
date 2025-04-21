#include "standalone_dashboard.h"
#include <QApplication>
#include <QStyleFactory>
#include <QMessageBox>
#include <QProcess>
#include <iostream>
#include <unistd.h>

int main(int argc, char *argv[])
{
    // Check if running with admin privileges for proper system resource access
    #ifdef Q_OS_UNIX
    if (geteuid() != 0) {
        // Not running as root, restart with sudo
        QMessageBox::information(nullptr, "Elevated Privileges Required",
                                "This application needs admin privileges to access system resources.\n"
                                "The application will now restart with sudo.");

        // Build command to restart with sudo
        QString program = "sudo";
        QStringList arguments;
        arguments << "-E" << argv[0];  // -E preserves environment variables
        
        // Add any command line args
        for (int i = 1; i < argc; ++i) {
            arguments << argv[i];
        }
        
        // Start the process
        QProcess::startDetached(program, arguments);
        return 0;  // Exit this instance
    }
    #endif

    QApplication a(argc, argv);
    
    // Set fusion style for a modern look
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    
    // Create dark palette
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    
    QApplication::setPalette(darkPalette);
    
    // Create and show the dashboard
    try {
        StandaloneDashboard w;
        w.show();
        return a.exec();
    } catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "Error", 
                             QString("Failed to initialize dashboard: %1").arg(e.what()));
        return 1;
    }
}
