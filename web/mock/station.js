const chargeData = {
  xAxis: ['周一', '周二', '周三', '周四', '周五', '周六', '周日'],
  curWeek: [150, 170, 160, 180, 170, 190, 180],
  lastWeek: [150, 140, 130, 160, 150, 140, 160]
}

const batteryData = {

}

const statData = {
  xAxis: ['6-12 0时', '4时', '8时', '12时', '16时', '20时', '6-13 0时', '4时', '8时', '12时', '16时', '20时', '6-14 0时', '4时', '8时', '12时', '16时', '20时'],
  connects: [5, 2, 4, 12, 18, 34, 6, 0, 5, 11, 19, 32, 5, 2, 5, 12, 19, 35],
  warns: [0, 0, 0, 1, 2, 5, 0, 0, 0, 2, 3, 5, 0, 1, 5, 4, 3, 8],
  uploads: [80, 80, 87, 94, 97, 116, 82, 80, 83, 86, 90, 110, 81, 80, 84, 91, 95, 112]
}

const stationInfo = {
  name: '咸水沽12站',
  id: '001',
  administrator: '咸水沽储能管理员',
  position: '津南区同砚路135号',
  deals: 109,
  delivered: 63,
  pending: 46,
  left: 5,
  number: [25, 30, 35],
  status: 1
}

const warnings = [
  {
    warn: '温度-40℃',
    bmuId: '00001',
    pointId: '00001-1',
    uploadTime: '2021-06-13 14:26',
    level: 1
  },
  {
    warn: '温度-50℃',
    bmuId: '00001',
    pointId: '00001-1',
    uploadTime: '2021-06-12 14:29',
    level: 2
  },
  {
    warn: '温度-70℃',
    bmuId: '00001',
    pointId: '00001-1',
    uploadTime: '2021-06-12 15:43',
    level: 3
  }
]

const userInfo = {
  img: 'https://wpimg.wallstcn.com/f778738c-e4f8-4870-b634-56703b4acafe.gif',
  name: '咸水沽储能管理员',
  stationTable: [
    {
      name: '咸水沽12站',
      id: '00005584',
      status: 1
    },
    {
      name: '天津大学2站',
      id: '00002515',
      status: 2
    },
    {
      name: '西青区3站',
      id: '00502516',
      status: 1
    }
  ]
}

const slaveTable = [
  {
    name: '1号BCU',
    status: 'online',
    id: '00005584001',
    administrator: '咸水沽储能管理员',
    duration: '0.5h',
    online: '2021-06-13 10:41',
    warning: 0,
    left: 35
  },
  {
    name: '2号BCU',
    status: 'offline',
    id: '00005584002',
    administrator: '咸水沽储能管理员',
    duration: '8.9h',
    online: '2021-06-12 10:41',
    warning: 5,
    left: 85
  },
  {
    name: '3号BCU',
    status: 'warn',
    id: '00005584003',
    administrator: '咸水沽储能管理员',
    duration: '0.25h',
    online: '2021-06-12 20:41',
    warning: 1,
    left: 53
  }
]

const warningRank = [
  {
    content: 'BMU ID-001',
    addition: '锂电池48串',
    tagType: 'danger',
    tagContent: '35',
    email: "锂电池48串@dazto.com",
  },
  {
    content: 'BMU ID-005',
    addition: '锌锰电池60串',
    tagType: 'warning',
    tagContent: '21',
    email: "锌锰电池60串@dazto.com",
  },
  {
    content: 'BMU ID-004',
    addition: '钴镍电池16串',
    tagType: 'success',
    tagContent: '5',
    email: "钴镍电池16串@dazto.com",
  }
]

const connectRank = [
  {
    content: 'BMU ID-005',
    addition: '锌锰电池60串',
    tagType: 'success',
    tagContent: '68',
    email: "锌锰电池60串@dazto.com",
  },
  {
    content: 'BMU ID-003',
    addition: '碱性电池24串',
    tagType: 'success',
    tagContent: '57',
    email: "碱性电池24串@dazto.com",
  },
  {
    content: 'BMU ID-001',
    addition: '锂电池48串',
    tagType: 'success',
    tagContent: '42',
    email: "锂电池48串@dazto.com",
  }
]

const dischargeRank = [
  {
    content: 'BMU ID-005',
    addition: '锌锰电池60串',
    tagType: 'success',
    tagContent: '2.1K',
    email: "锌锰电池60串@dazto.com",
  },
  {
    content: 'BMU ID-001',
    addition: '锂电池48串',
    tagType: 'success',
    tagContent: '1.5K',
    email: "锂电池48串@dazto.com",
  },
  {
    content: 'BMU ID-002',
    addition: '碱性电池32串',
    tagType: 'success',
    tagContent: '1.1K',
    email: "碱性电池32串@dazto.com",
  }
]

const collects = {}
const connects = {}

module.exports = [
  {
    url: '/station/weekCharging',
    type: 'get',
    response: config => {
      return {
        code: 20000,
        data: chargeData
      }
    }
  },
  {
    url: '/station/batteryStatus',
    type: 'get',
    response: config => {
      return {
        code: 20000,
        data: batteryData
      }
    }
  },
  {
    url: '/station/statData',
    type: 'get',
    response: config => {
      return {
        code: 20000,
        data: statData
      }
    }
  },
  {
    url: '/station/warningRank',
    type: 'get',
    response: config => {
      return {
        code: 20000,
        data: warningRank
      }
    }
  },
  {
    url: '/station/connectRank',
    type: 'get',
    response: config => {
      return {
        code: 20000,
        data: connectRank
      }
    }
  },
  {
    url: '/station/dischargeRank',
    type: 'get',
    response: config => {
      return {
        code: 20000,
        data: dischargeRank
      }
    }
  },
  {
    url: '/station/userInfo',
    type: 'get',
    response: config => {
      return {
        code: 20000,
        data: userInfo
      }
    }
  },
  {
    url: '/station/stationInfo',
    type: 'get',
    response: config => {
      return {
        code: 20000,
        data: stationInfo
      }
    }
  },
  {
    url: '/station/warningRecord',
    type: 'get',
    response: config => {
      return {
        code: 20000,
        data: warnings
      }
    }
  },
  {
    url: '/station/slaveTable',
    type: 'get',
    response: config => {
      return {
        code: 20000,
        data: slaveTable,
        title: '下级从属BCU'
      }
    }
  },
  {
    url: '/station/connectRecord',
    type: 'get',
    response: config => {
      return {
        code: 20000,
        data: connects
      }
    }
  },
  {
    url: '/station/collectRecord',
    type: 'get',
    response: config => {
      return {
        code: 20000,
        data: collects
      }
    }
  }
]
