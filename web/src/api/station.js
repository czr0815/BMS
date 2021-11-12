import request from '@/utils/request'
import { getToken } from '@/utils/auth'

const token = getToken()

export function getStationDischargeScatter(stationId) {
  return request({
    url: '/api/v1/getStationDischargeScatter',
    method: 'get',
    params: {
      stationId: stationId,
      token: token
    }
  })
}

export function getStationRank() {
  return request({
    url: '/api/v1/getStationRank',
    method: 'get',
    params: {
      token: token
    }
  })
}

export function getStationSlaveTable(stationId) {
  return request({
    url: '/api/v1/getStationSlaves',
    method: 'get',
    params: {
      stationId: stationId,
      token: token
    }
  })
}

export function getStationInfos() {
  return request({
    url: '/api/v1/getStationInfos',
    method: 'get',
    params: {
      token: token
    }
  })
}

export function updateStation(stationId, stationForm) {
  var stationform = {
    name: stationForm.name,
    employeeId: stationForm.employeeId+"",
    position: stationForm.position,
    lng: stationForm.center.lng+"",
    lat: stationForm.center.lat+""
  }
  return request({
    url: '/api/v1/updateStation/'+stationId,
    method: 'put',
    params: {
      token: token
    },
    data: stationform
  })
}

export function addStation(stationCreate){
  var stationcreate = {
    name: stationCreate.name,
    employeeId: stationCreate.employeeId+"",
    position: stationCreate.position,
    lng: stationCreate.center.lng+"",
    lat: stationCreate.center.lat+""
  }
  return request({
    url: 'api/v1/addStation',
    method: 'post',
    params: {
      token: token
    },
    data: stationcreate
  })
}