#pragma once

// Qt
#include <QDialog>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QApplication>
#include <QMessageBox>
#include <QTextCodec>
#include <QAbstractListModel>
#include <QThread>

// C++
#include <memory>
#include <thread>
#include <condition_variable>

#ifndef NDEBUG

#define VERIFY(Connection) assert(Connection)

#else

#define VERIFY(Connection) Connection

#endif