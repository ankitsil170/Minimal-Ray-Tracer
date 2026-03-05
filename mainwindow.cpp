#include "mainwindow.h"
#include <QApplication>
#include <QPainter>
#include <QSizePolicy>

// ═══════════════════════════════════════════════
//  RenderWorker
// ═══════════════════════════════════════════════
void RenderWorker::run() {
    auto world = (sceneIndex == 0)
    ? SceneBuilder::buildDefault()
    : SceneBuilder::buildRandom();

    Camera cam = [&]() -> Camera {
        if (sceneIndex == 0) {
            return Camera(
                Point3(3, 1.5, 2), Point3(0, 0, -1), Vec3(0,1,0),
                40.0, (double)width/height, 0.05, 3.5
                );
        } else {
            return Camera(
                Point3(10, 2, 3), Point3(0, 0.5, -1), Vec3(0,1,0),
                30.0, (double)width/height, 0.08, 10.0
                );
        }
    }();

    renderer->imageWidth     = width;
    renderer->imageHeight    = height;
    renderer->samplesPerPixel = samples;
    renderer->maxDepth       = depth;

    renderer->render(world, cam,
                     [this](int done, int total) {
                         emit progressUpdate(done, total);
                     },
                     [this](int x, int y, uint8_t r, uint8_t g, uint8_t b) {
                         emit pixelReady(x, y, r, g, b);
                     }
                     );

    emit finished(renderer->cancelled.load());
}

// ═══════════════════════════════════════════════
//  MainWindow
// ═══════════════════════════════════════════════
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Minimal C++ Ray Tracer");
    buildUi();
    resize(1100, 720);
}

MainWindow::~MainWindow() {
    if (worker && worker->isRunning()) {
        worker->renderer->cancelled = true;
        worker->wait(3000);
    }
}

void MainWindow::buildUi() {
    // ── Central widget ────────────────────────
    auto* central = new QWidget;
    setCentralWidget(central);
    auto* rootLayout = new QHBoxLayout(central);
    rootLayout->setSpacing(0);
    rootLayout->setContentsMargins(0, 0, 0, 0);

    // ── Left: render canvas ───────────────────
    scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    imageLabel = new QLabel;
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setText("← Configure settings and click Render");
    imageLabel->setStyleSheet("color: #888; font-size: 14px;");
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    scrollArea->setWidget(imageLabel);
    scrollArea->setWidgetResizable(true);

    // ── Right: control panel ──────────────────
    auto* panel = new QWidget;
    panel->setFixedWidth(260);
    panel->setStyleSheet(R"(
        QWidget { background: #1e1e2e; color: #cdd6f4; font-family: 'Consolas', monospace; }
        QGroupBox { border: 1px solid #313244; border-radius: 6px; margin-top: 12px;
                    font-weight: bold; color: #cba6f7; padding: 8px; }
        QGroupBox::title { subcontrol-origin: margin; left: 8px; padding: 0 4px; }
        QSpinBox, QComboBox { background: #313244; border: 1px solid #45475a;
                              border-radius: 4px; padding: 4px 6px; color: #cdd6f4; }
        QSpinBox::up-button, QSpinBox::down-button { background: #45475a; border: none; }
        QPushButton { border-radius: 5px; padding: 8px; font-weight: bold; font-size: 13px; }
        QPushButton#renderBtn { background: #89b4fa; color: #1e1e2e; }
        QPushButton#renderBtn:hover { background: #74c7ec; }
        QPushButton#cancelBtn { background: #f38ba8; color: #1e1e2e; }
        QPushButton#saveBtn { background: #a6e3a1; color: #1e1e2e; }
        QPushButton:disabled { background: #45475a; color: #585b70; }
        QProgressBar { border: 1px solid #45475a; border-radius: 4px; background: #313244;
                       text-align: center; color: #cdd6f4; height: 18px; }
        QProgressBar::chunk { background: #89b4fa; border-radius: 3px; }
        QLabel { color: #bac2de; }
    )");

    auto* panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(12, 16, 12, 16);
    panelLayout->setSpacing(12);

    // Title
    auto* title = new QLabel("⚡ Ray Tracer");
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: #cba6f7; letter-spacing: 1px;");
    title->setAlignment(Qt::AlignCenter);
    panelLayout->addWidget(title);

    auto* subtitle = new QLabel("CPU path tracer · C++17 + Qt");
    subtitle->setStyleSheet("font-size: 10px; color: #6c7086;");
    subtitle->setAlignment(Qt::AlignCenter);
    panelLayout->addWidget(subtitle);

    // Scene group
    auto* sceneGroup = new QGroupBox("Scene");
    auto* sceneLayout = new QVBoxLayout(sceneGroup);
    sceneCombo = new QComboBox;
    sceneCombo->addItem("Classic — 3 Spheres");
    sceneCombo->addItem("Random — Many Spheres");
    sceneLayout->addWidget(sceneCombo);
    panelLayout->addWidget(sceneGroup);

    // Resolution group
    auto* resGroup = new QGroupBox("Resolution");
    auto* resForm  = new QFormLayout(resGroup);
    resForm->setSpacing(6);
    widthSpin  = new QSpinBox; widthSpin->setRange(100, 1920); widthSpin->setValue(800); widthSpin->setSingleStep(100);
    heightSpin = new QSpinBox; heightSpin->setRange(100, 1080); heightSpin->setValue(450); heightSpin->setSingleStep(50);
    resForm->addRow("Width:",  widthSpin);
    resForm->addRow("Height:", heightSpin);
    panelLayout->addWidget(resGroup);

    // Quality group
    auto* qualGroup = new QGroupBox("Quality");
    auto* qualForm  = new QFormLayout(qualGroup);
    qualForm->setSpacing(6);
    samplesSpin = new QSpinBox; samplesSpin->setRange(1, 500); samplesSpin->setValue(50);
    depthSpin   = new QSpinBox; depthSpin->setRange(1, 50);   depthSpin->setValue(10);
    qualForm->addRow("Samples/px:", samplesSpin);
    qualForm->addRow("Max depth:",  depthSpin);
    panelLayout->addWidget(qualGroup);

    // Buttons
    renderBtn = new QPushButton("▶  Render");
    renderBtn->setObjectName("renderBtn");
    cancelBtn = new QPushButton("■  Cancel");
    cancelBtn->setObjectName("cancelBtn");
    cancelBtn->setEnabled(false);
    saveBtn   = new QPushButton("💾  Save PNG");
    saveBtn->setObjectName("saveBtn");
    saveBtn->setEnabled(false);

    panelLayout->addWidget(renderBtn);
    panelLayout->addWidget(cancelBtn);
    panelLayout->addWidget(saveBtn);

    // Progress
    progressBar = new QProgressBar;
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    panelLayout->addWidget(progressBar);

    timeLabel = new QLabel("Time: —");
    timeLabel->setAlignment(Qt::AlignCenter);
    timeLabel->setStyleSheet("font-size: 11px; color: #6c7086;");
    panelLayout->addWidget(timeLabel);

    panelLayout->addStretch();

    // Legend
    auto* legendLabel = new QLabel(
        "Materials:\n"
        "  🔴 Lambertian (matte)\n"
        "  ⚪ Metal (reflective)\n"
        "  🔵 Dielectric (glass)"
        );
    legendLabel->setStyleSheet("font-size: 10px; color: #6c7086; line-height: 160%;");
    panelLayout->addWidget(legendLabel);

    rootLayout->addWidget(scrollArea, 1);
    rootLayout->addWidget(panel);

    // Connections
    connect(renderBtn, &QPushButton::clicked, this, &MainWindow::startRender);
    connect(cancelBtn, &QPushButton::clicked, this, &MainWindow::cancelRender);
    connect(saveBtn,   &QPushButton::clicked, this, &MainWindow::saveImage);

    clockTimer = new QTimer(this);
    connect(clockTimer, &QTimer::timeout, this, [this]() {
        if (worker && worker->isRunning()) {
            qint64 ms = elapsedTimer.elapsed();
            timeLabel->setText(QString("Time: %1.%2s")
                                   .arg(ms/1000).arg((ms%1000)/100));
        }
    });
}

void MainWindow::startRender() {
    int w = widthSpin->value();
    int h = heightSpin->value();

    renderImage = QImage(w, h, QImage::Format_RGB888);
    renderImage.fill(Qt::black);
    imageLabel->setPixmap(QPixmap::fromImage(renderImage).scaled(
        scrollArea->width()-4, scrollArea->height()-4,
        Qt::KeepAspectRatio, Qt::SmoothTransformation));

    renderBtn->setEnabled(false);
    cancelBtn->setEnabled(true);
    saveBtn->setEnabled(false);
    progressBar->setValue(0);
    timeLabel->setText("Time: 0.0s");

    auto sharedRenderer = std::make_shared<Renderer>();

    worker = new RenderWorker;
    worker->width       = w;
    worker->height      = h;
    worker->samples     = samplesSpin->value();
    worker->depth       = depthSpin->value();
    worker->sceneIndex  = sceneCombo->currentIndex();
    worker->renderer    = sharedRenderer;

    connect(worker, &RenderWorker::pixelReady,      this, &MainWindow::onPixelReady,      Qt::QueuedConnection);
    connect(worker, &RenderWorker::progressUpdate,  this, &MainWindow::onProgress,         Qt::QueuedConnection);
    connect(worker, &RenderWorker::finished,        this, &MainWindow::onRenderFinished,   Qt::QueuedConnection);
    connect(worker, &RenderWorker::finished,        worker, &QObject::deleteLater);

    elapsedTimer.start();
    clockTimer->start(100);
    worker->start();

    statusBar()->showMessage("Rendering...");
}

void MainWindow::cancelRender() {
    if (worker) worker->renderer->cancelled = true;
}

void MainWindow::onPixelReady(int x, int y, int r, int g, int b) {
    if (y < 0 || y >= renderImage.height() || x < 0 || x >= renderImage.width()) return;
    renderImage.setPixel(x, y, qRgb(r, g, b));

    // Refresh display periodically
    if (x % 80 == 0) {
        imageLabel->setPixmap(QPixmap::fromImage(renderImage).scaled(
            scrollArea->width()-4, scrollArea->height()-4,
            Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void MainWindow::onProgress(int done, int total) {
    int pct = total > 0 ? (done * 100 / total) : 0;
    progressBar->setValue(pct);
}

void MainWindow::onRenderFinished(bool cancelled) {
    clockTimer->stop();
    qint64 ms = elapsedTimer.elapsed();

    renderBtn->setEnabled(true);
    cancelBtn->setEnabled(false);
    saveBtn->setEnabled(!cancelled);

    imageLabel->setPixmap(QPixmap::fromImage(renderImage).scaled(
        scrollArea->width()-4, scrollArea->height()-4,
        Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QString msg = cancelled
                      ? QString("Cancelled after %1.%2s").arg(ms/1000).arg((ms%1000)/100)
                      : QString("Done in %1.%2s").arg(ms/1000).arg((ms%1000)/100);

    timeLabel->setText(msg);
    progressBar->setValue(cancelled ? progressBar->value() : 100);
    statusBar()->showMessage(msg);
    worker = nullptr;
}

void MainWindow::saveImage() {
    QString path = QFileDialog::getSaveFileName(
        this, "Save Image", "render.png",
        "PNG (*.png);;PPM (*.ppm);;JPEG (*.jpg)"
        );
    if (path.isEmpty()) return;

    if (renderImage.save(path))
        statusBar()->showMessage("Saved: " + path);
    else
        QMessageBox::warning(this, "Save Error", "Could not save file.");
}
