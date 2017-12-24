#ifndef WEBSERVICE_H
#define WEBSERVICE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <functional>
#include <memory>

#include "workspacesmodel.h"
#include "projectsmodel.h"
#include "tasksmodel.h"
#include "timeentriesmodel.h"
#include <QJsonValue>

struct Response
{
    bool isError = false;
    QString errorString = "";
    int statusCode = 0;
    QByteArray data;
    QMap<QString, QString> headerData;
};

typedef std::shared_ptr<Response> ResponsePtr;

typedef std::function<void (ResponsePtr)> PerformCallback;
typedef std::function<void (bool, QString)> SuccessCallback;

using namespace Entities;

class WebService : public QObject
{
    Q_OBJECT
public:
    explicit WebService(QObject *parent = nullptr);
    ~WebService();

    void createUser(const QString &username, const QString &email, UserPtr user, SuccessCallback successCallback);

    void updateAllEntities(const QString &ownerId,
                           WorkspacesModelPtr workspaceModel,
                           ProjectsModelPtr projectModel,
                           TasksModelPtr taskModel,
                           TimeEntriesModelPtr timeModel,
                           SuccessCallback successCallback);

    void getAllWorkspaces(const QString &ownerId, WorkspacesModelPtr workspaceModel, SuccessCallback successCallback);

    void getAllProjects(const QString &ownerId, ProjectsModelPtr projectModel, SuccessCallback successCallback);

    void getAllTasks(const QString &ownerId, TasksModelPtr taskModel, SuccessCallback successCallback);

    void getAllTimeEntries(const QString &ownerId, TimeEntriesModelPtr timeModel, SuccessCallback successCallback);

    void createWorkspace(const QString &name, const QString &ownerId, SuccessCallback successCallback);

    void deleteWorkspace(const QString &workspaceId, SuccessCallback successCallback);

    void createProject(const QString &name, const QString &workspaceId, SuccessCallback successCallback);

    void deleteProject(const QString &projectId, SuccessCallback successCallback);

    void createTask(const QString &name, const QString &projectId, TaskPtr task, SuccessCallback successCallback);

    void deleteTask(const QString &taskId, SuccessCallback successCallback);

    void startTask(const QString &taskId, TimeEntryPtr timeEntry, SuccessCallback successCallback);

    void stopTimeEntry(TimeEntryPtr timeEntry, SuccessCallback successCallback);

    //TODO: authorization

signals:

public slots:


private:
    void getRequest(const QNetworkRequest& request, PerformCallback callback);

    void postRequest(const QString &query, PerformCallback callback, QJsonValue vars = QJsonValue());

    void requestFunction(QNetworkReply * reply, PerformCallback callback);



private:
    QNetworkAccessManager m_manager;
    QString m_host;
    int m_waitTime;
};

#endif // WEBSERVICE_H
