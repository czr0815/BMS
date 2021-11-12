import request from '@/utils/request'
import { getToken } from '@/utils/auth'

const token = getToken()

export function getCollectWeek() {
  return request({
    url: 'http://localhost:8000/api/v1/getCollectWeek?stationId=',
    methods: 'get'
  })
}

export function getCollectOverview() {
  return request({
    url: 'http://localhost:8000/api/v1/getCollectOverview?stationId=',
    methods: 'get'
  })
}

export function getCollectsByStationId() {
  return request({
    url: 'http://localhost:8000/api/v1/getCollects?stationId=',
    methods: 'get'
  })
}

export function getStationsForUser() {
  return request({
    url: 'http://localhost:8000/api/v1/getStationsForUser?userId=',
    methods: 'get'
  })
}

export function getCollects(tableForum) {
  return request({
    url: 'api/v1/getCollects?device='+tableForum.device+'&deviceId='+tableForum.deviceId+'&valueType='+tableForum.valueType+'&startTime='+tableForum.timeRange[0]+'&endTime='+tableForum.timeRange[1],
    methods: 'get',
    params: {
      token: token
    }
  })
}