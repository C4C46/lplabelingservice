#pragma once

#include "lpmqservice.h"
#include "LPLabelingServicePrivate.h"
#include <QString>

// 新的消息类型
enum {
	LP_LITHIUM_LABELING_TAG = 1001, // 假设消息类型编号
};

struct Lithium_LabelingTag {
	QString unitName;
	qreal centYPosMm;
	qreal centXPosMm;
	int channelId;
	QString checkType;
};

// QDataStream 序列化操作符
inline QDataStream &operator<<(QDataStream &out, const Lithium_LabelingTag &tag) {
	out << tag.unitName << tag.centYPosMm << tag.centXPosMm << tag.channelId << tag.checkType;
	return out;
}

// QDataStream 反序列化操作符
inline QDataStream &operator>>(QDataStream &in, Lithium_LabelingTag &tag) {
	in >> tag.unitName >> tag.centYPosMm >> tag.centXPosMm >> tag.channelId >> tag.checkType;
	return in;
}


class LPLabelingServicePrivate;
class LPMQSERVICE_API lplabelingservice : public LPMQService
{
public:
	lplabelingservice(QString strName);
	void sendHeartInfo();
	virtual void recvMsg(QByteArray& ba, const MsgReceiverInfo& receiverInfo);
private:
	void handleCameraWaveData(QDataStream& ds, const MsgReceiverInfo& receiverInfo);
	void handleIpuStatus(QDataStream& ds, const MsgReceiverInfo& receiverInfo);
	void handleLabelingTag(QDataStream & ds, const MsgReceiverInfo & receiverInfo);
	void forwardLabelingTag(const Lithium_LabelingTag & tagInfo);
private:
	QScopedPointer<LPLabelingServicePrivate> d;
	QMutex m_mutex;
	QMap<int, qint64> m_camWavePeriod;
	QMap<QString, qint64> m_camStatusPeriod;
};
