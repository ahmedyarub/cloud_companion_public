#ifndef CLOUD_COMPANION_INSTANCES_TABLE_MODEL_HPP
#define CLOUD_COMPANION_INSTANCES_TABLE_MODEL_HPP

#include <QAbstractTableModel>
#include <QTimer>

#include "../ec2/InstanceInfo.hpp"

namespace CloudCompanion {
  class InstancesTableModel final : public QAbstractTableModel {
    enum ec2_attributes { instance_id = Qt::UserRole + 1, ami, instance_type, status, actions };

    Q_OBJECT
  private:
    QVector<InstanceInfo> instances;
    QTimer timer;

  public:
    explicit InstancesTableModel(QObject *parent = nullptr) noexcept;

    void updateTimer();

    [[nodiscard]] int rowCount(const QModelIndex &) const override;

    [[nodiscard]] int columnCount(const QModelIndex &) const noexcept override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation,
                                                  int role) const override;

  public slots:

    [[maybe_unused]] void btnRefreshInstancesClick();

    [[maybe_unused]] void btnSelectInstanceClick(qsizetype index);

    [[maybe_unused]] void btnStartStopClick(qsizetype index);

  signals:
    void ec2ListUpdated(QVector<CloudCompanion::InstanceInfo>);

    void ec2Selected(QString);

    void showMessage(QString message, int timeout);
  };
}  // namespace CloudCompanion

#endif  // CLOUD_COMPANION_INSTANCES_TABLE_MODEL_HPP
