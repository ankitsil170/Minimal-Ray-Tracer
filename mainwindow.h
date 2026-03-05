#pragma once
#include <QMainWindow>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QPushButton>
#include <QComboBox>
#include <QProgressBar>
#include <QImage>
#include <QPixmap>
#include <QThread>
#include <QTimer>
#include <QStatusBar>
#include <QScrollArea>
#include <QGroupBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QElapsedTimer>
#include <memory>
#include <atomic>
#include "rendering/renderer.h"
#include "rendering/scene_builder.h"
#include "core/camera.h"

// ── Worker thread ─────────────────────────────
class RenderWorker : public QThread {
    Q_OBJECT
public:
    int  width, height, samples, depth;
    int  sceneIndex;
    std::shared_ptr<Renderer> renderer;

    void run() override;

signals:
    void pixelReady(int x, int y, int r, int g, int b);
    void progressUpdate(int done, int total);
    void finished(bool cancelled);
};

// ── Main window ───────────────────────────────
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void startRender();
    void cancelRender();
    void onPixelReady(int x, int y, int r, int g, int b);
    void onProgress(int done, int total);
    void onRenderFinished(bool cancelled);
    void saveImage();

private:
    void buildUi();
    Camera makeCamera(int w, int h, int sceneIdx);

    // UI widgets
    QLabel*       imageLabel;
    QScrollArea*  scrollArea;
    QSpinBox*     widthSpin;
    QSpinBox*     heightSpin;
    QSpinBox*     samplesSpin;
    QSpinBox*     depthSpin;
    QComboBox*    sceneCombo;
    QPushButton*  renderBtn;
    QPushButton*  cancelBtn;
    QPushButton*  saveBtn;
    QProgressBar* progressBar;
    QLabel*       timeLabel;

    QImage        renderImage;
    RenderWorker* worker{nullptr};
    QElapsedTimer elapsedTimer;
    QTimer*       clockTimer{nullptr};
};
