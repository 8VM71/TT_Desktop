#include "logiccore.h"
#include <QDebug>
#include <QLoggingCategory>
#include <QMetaType>
#include <QTimerEvent>
#include <QDateTime>

Q_LOGGING_CATEGORY(logicCore, "LogicCore")

LogicCore::LogicCore(QObject *parent)
    : QObject(parent)
    , m_timerDuration("00:00:00")
    , m_running(false)
    , m_timerId(-1)
{
    m_currentUser.id = "42cfb602-f544-4b1b-b01d-63cd6a0b644f";
    m_workspacesModel = std::make_shared<WorkspacesModel>();
    m_projectModel = std::make_shared<ProjectsModel>();
    m_tasksModel = std::make_shared<TasksModel>();
    m_timeEntriesModel = std::make_shared<TimeEntriesModel>();

    updateWorkspacesModel();
    updateProjectsModel();
    updateTasksModel();
    updateTimeEntriesModel();
}

LogicCore::~LogicCore()
{

}

WorkspacesModel *LogicCore::workspacesModel() const
{
    return m_workspacesModel.get();
}

ProjectsModel *LogicCore::projectsModel() const
{
    return m_projectModel.get();
}

TasksModel *LogicCore::tasksModel() const
{
    return m_tasksModel.get();
}

TimeEntriesModel *LogicCore::timeEntriesModel() const
{
    return m_timeEntriesModel.get();
}

QString LogicCore::timerDuration() const
{
    return m_timerDuration;
}

bool LogicCore::running() const
{
    return m_running;
}

void LogicCore::startNewTask(const QString &taskName, const QString &projectId)
{
    m_currentTask = std::make_shared<Task>();

    m_webService.createTask(taskName, projectId, m_currentTask, [this](bool success, QString info){
        qCDebug(logicCore) << QString("Create task success: %0, info: %1").arg(success).arg(info);
        if (success)
        {
            this->startExistTask(m_currentTask->id);
        }

    });
}

void LogicCore::stopTask()
{
    //TODO: implement
}

void LogicCore::startExistTask(const QString &taskId)
{
    m_currentTimeEntry = std::make_shared<TimeEntry>();

    m_webService.startTask(taskId, m_currentTimeEntry, [this](bool success, QString info){
        qCDebug(logicCore) << QString("Start task success: %0, info: %1").arg(success).arg(info);
        if (success)
        {
            if (m_timerId != -1)
            {
                killTimer(m_timerId);
                m_timerId = -1;
            }
            m_running = true;
            emit this->runningChanged(m_running);
            this->updateTimerDuration();

            m_timerId = startTimer(1000);
        }
    });
}


void LogicCore::deleteTask()
{
    //TODO: implement
}

void LogicCore::deleteTimeEntry()
{
    //TODO: implement
}

void LogicCore::updateTask()
{
    //TODO: implement
}

void LogicCore::updateTimeEntry()
{
    //TODO: implement
}

void LogicCore::setProjectForTask()
{
    //TODO: implement
}

void LogicCore::createNewProject(const QString &name, const QString &workspaceId)
{
    m_webService.createProject(name, workspaceId, [this, name, workspaceId](bool success, QString id){
        if(success)
        {
            m_projectModel->addItem(id, name, workspaceId);
        }
    });
}

void LogicCore::moveProjectToWorkspace()
{
    //TODO: implement
}

void LogicCore::updateProject()
{
    //TODO: implement
}

void LogicCore::deleteProject(const QString &projectId)
{
    m_webService.deleteProject(projectId, [this, projectId](bool success, QString info){
        if(success)
            m_projectModel->removeItem(projectId);
        else
            qCDebug(logicCore) << "Project delete error:" << info;
    });
}

void LogicCore::setProjectAsDefault()
{
    //TODO: implement
}

void LogicCore::createNewWorkspace(const QString &name)
{
    m_webService.createWorkspace(name, m_currentUser.id, [this, name](bool success, QString id){
        if(success)
        {
            m_workspacesModel->addItem(id, name, m_currentUser.id);
        }
    });
}

void LogicCore::updateWorkspace()
{
    //TODO: implement
}

void LogicCore::deleteWorkspace(const QString &workspaceId)
{
    m_webService.deleteWorkspace(workspaceId, [this, workspaceId](bool success, QString info){
        if(success)
            m_workspacesModel->removeItem(workspaceId);
        else
            qCDebug(logicCore) << "Workspace delete error:" << info;
    });
}

void LogicCore::setWorkspaceAsDefault()
{
    //TODO: implement
}

void LogicCore::updateWorkspacesModel()
{
    m_webService.getAllWorkspaces(m_currentUser.id, m_workspacesModel, [](bool succes, QString info){
        qCDebug(logicCore) << QString("Update workspaces success: %0, info: %1").arg(succes).arg(info);
    });
}

void LogicCore::updateProjectsModel()
{
    m_webService.getAllProjects(m_currentUser.id, m_projectModel, [this](bool succes, QString info){
        qCDebug(logicCore) << QString("Update projects success: %0, info: %1").arg(succes).arg(info);
        emit this->projectsModelChanged();
    });
}

void LogicCore::updateTasksModel()
{
    m_webService.getAllTasks(m_currentUser.id, m_tasksModel, [](bool succes, QString info){
        qCDebug(logicCore) << QString("Update tasks success: %0, info: %1").arg(succes).arg(info);
    });
}

void LogicCore::updateTimeEntriesModel()
{
    m_webService.getAllTimeEntries(m_currentUser.id, m_timeEntriesModel, [](bool succes, QString info){
        qCDebug(logicCore) << QString("Update time entries success: %0, info: %1").arg(succes).arg(info);
    });
}

void LogicCore::updateTimerDuration()
{
    qint64 duration = std::abs(QDateTime::currentDateTime().toMSecsSinceEpoch() - m_currentTimeEntry->startMSecsSinceEpoch);

    QTime time(0, 0, 0, 0);
    time = time.addMSecs(static_cast<int>(duration));
    m_timerDuration = time.toString("hh:mm:ss");

    emit this->timerDurationChanged(m_timerDuration);
}


void LogicCore::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_timerId)
    {
        updateTimerDuration();
    }
}
