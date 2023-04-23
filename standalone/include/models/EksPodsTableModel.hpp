#ifndef CLOUD_COMPANION_EKS_PODS_TABLE_MODEL_HPP
#define CLOUD_COMPANION_EKS_PODS_TABLE_MODEL_HPP

#include <QAbstractTableModel>
#include <QTimer>
#include <QtQmlIntegration>
#include <kubernetes/PodInfo.hpp>

#include "../ec2/InstanceInfo.hpp"
#include "eks/Eks.hpp"

namespace CloudCompanion {
  class EksPodsTableModel final : public QAbstractTableModel {
    enum eks_attributes { name = Qt::UserRole + 1, ns, pod_ip, created_at, status, actions };

    Q_OBJECT
    QML_ELEMENT

  private:
    QVector<PodInfo> pods;
    Eks eks;

  public:
    explicit EksPodsTableModel(QObject *parent = nullptr) noexcept;

    [[nodiscard]] int rowCount(const QModelIndex &) const override;

    [[nodiscard]] int columnCount(const QModelIndex &) const noexcept override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation,
                                                  int role) const override;

  public slots:
    [[maybe_unused]] void btnRefreshPodsClick();
    [[maybe_unused]] void btnWatchPodsClick(bool stop);
    [[maybe_unused]] void btnDelete(int index);
    [[maybe_unused]] void btnLog(int index);
    [[maybe_unused]] void podAdded(const PodInfo &podInfo);
    [[maybe_unused]] void podModified(const PodInfo &podInfo);
    [[maybe_unused]] void podRemoved(QString podUuid);
  };
}  // namespace CloudCompanion

#endif  // CLOUD_COMPANION_EKS_PODS_TABLE_MODEL_HPP
