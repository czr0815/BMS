import request from '@/utils/request'
import { getToken } from '@/utils/auth'

const token = getToken()

export function getWarningStationStat() {
  return request({
      url: '/api/v1/getWarningStationStat',
      method: 'get',
      params: {
        token: token
      }
  })
}

export function getWarningTimeStat() {
  return request({
      url: '/api/v1/getWarningTimeStat',
      method: 'get',
      params: {
        token: token
      }
  })
}

export function getWarnings(warningForum) {
  return request({
    url: 'api/v1/getWarnings?device='+warningForum.device+'&deviceId='+warningForum.deviceId+'&valueType='+warningForum.valueType+'&startTime='+warningForum.timeRange[0]+'&endTime='+warningForum.timeRange[1],
    method: 'get',
    params: {
      token: token
    }
  })
}
