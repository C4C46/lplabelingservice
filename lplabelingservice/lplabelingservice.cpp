#include "lplabelingservice.h"

#include "lpservicestatusmsg.h"
#include "lpwavedatamsg.h"
#include <QTimer>
lplabelingservice* g_instance = nullptr;

lplabelingservice::lplabelingservice(QString strName)
	:LPMQService(strName),
	d(new LPLabelingServicePrivate)
{
	d->heart_timer_ptr_ = QSharedPointer<LPTaskTimer>(new LPTaskTimer());
	d->heart_timer_ptr_->registTask("heart", std::bind(&lplabelingservice::sendHeartInfo, this));
	g_instance = this;
	d->heart_timer_ptr_->start();
}

void lplabelingservice::sendHeartInfo()
{
	//QMutexLocker locker(&m_mutex);
	lpbs::CameraWaveStatusInfo heartInfo;
	heartInfo.serviceName = LPMQService::name();
	QByteArray ba;
	QDataStream out(&ba, QIODevice::WriteOnly);
	out << heartInfo;
	sendMsg(LP_CAM_WAVE_STATUS_INFO, ba);

}

void lplabelingservice::recvMsg(QByteArray & ba, const MsgReceiverInfo & receiverInfo)
{
	QDataStream in(ba);
	int type;
	in >> type;
	switch (type)
	{
	case LP_IPUMSG_PUB_WAVE_DATA:
	{
		handleCameraWaveData(in, receiverInfo);
		break;
	}
	case LP_IPUMSG_PUB_STATUS_INFO:
	{
		handleIpuStatus(in, receiverInfo);
		break;
	}

	default:
		break;
	}
}

void lplabelingservice::handleLabelingTag(QDataStream & ds, const MsgReceiverInfo & receiverInfo)
{
	Lithium_LabelingTag tagInfo;
	ds >> tagInfo;
	// 处理接收到的标签信息

	// 打印接收到的标签信息
	qDebug() << "Received Lithium_LabelingTag:";
	qDebug() << "  unitName:" << tagInfo.unitName;
	qDebug() << "  centYPosMm:" << tagInfo.centYPosMm;
	qDebug() << "  centXPosMm:" << tagInfo.centXPosMm;
	qDebug() << "  channelId:" << tagInfo.channelId;
	qDebug() << "  checkType:" << tagInfo.checkType;


	forwardLabelingTag(tagInfo);
	qDebug() << "yes " << __FUNCTION__;
}


void lplabelingservice::forwardLabelingTag(const Lithium_LabelingTag & tagInfo)
{
	// 转发逻辑
	QByteArray ba;
	QDataStream out(&ba, QIODevice::WriteOnly);
	out << tagInfo;
	sendMsg(LP_LITHIUM_LABELING_TAG, ba); // 假设下位机使用相同的消息类型
	qDebug() << "yes " << __FUNCTION__;
}

void lplabelingservice::handleCameraWaveData(QDataStream & ds, const MsgReceiverInfo & receiverInfo)
{
	lpbs::WaveData info;
	ds >> info;

	if (m_camWavePeriod.contains(info.camId) && (QDateTime::currentMSecsSinceEpoch() - m_camWavePeriod[info.camId] > 1000))
	{
		m_camWavePeriod.insert(info.camId, QDateTime::currentMSecsSinceEpoch());
	}
	else
	{
		if (m_camWavePeriod.contains(info.camId))
			return;
		m_camWavePeriod.insert(info.camId, QDateTime::currentMSecsSinceEpoch());
	}

	lpbs::CameraWaveForwarding camWaveForwardingInfo;
	camWaveForwardingInfo.data = info;
	QByteArray ba;
	QDataStream out(&ba, QIODevice::WriteOnly);
	out << camWaveForwardingInfo;
	sendMsg(LP_CAM_WAVE_FORWARDING, ba);
}

void lplabelingservice::handleIpuStatus(QDataStream & ds, const MsgReceiverInfo & receiverInfo)
{
	lpbs::IpuStatusInfo info;
	ds >> info;
	if (m_camStatusPeriod.contains(info.serviceName) && (QDateTime::currentMSecsSinceEpoch() - m_camStatusPeriod[info.serviceName] > 1000))
	{
		m_camStatusPeriod.insert(info.serviceName, QDateTime::currentMSecsSinceEpoch());
	}
	else
	{
		if (m_camStatusPeriod.contains(info.serviceName))
			return;
		m_camStatusPeriod.insert(info.serviceName, QDateTime::currentMSecsSinceEpoch());
	}

	lpbs::IpuStatusForwardingInfo cameraWaveStatusInfo;
	cameraWaveStatusInfo.ipuStatusInfo = info;
	QByteArray ba;
	QDataStream out(&ba, QIODevice::WriteOnly);
	out << cameraWaveStatusInfo;
	sendMsg(LP_IPU_STATUS_FORWARDING_INFO, ba);
}


LPMQService* LpMQServiceNewInstance(const QString& name)
{
	return new lplabelingservice(name);
}