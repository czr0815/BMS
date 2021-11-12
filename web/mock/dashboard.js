const LineData = {
    xAxis: ["周一", "周二", "周三", "周四", "周五", "周六", "周日"],
    curWeek: [150, 170, 160, 180, 170, 190, 180],
    lastWeek: [150, 140, 130, 160, 150, 140, 160]
}

const PieData = {

}

const userInfo = {
    img: "https://wpimg.wallstcn.com/f778738c-e4f8-4870-b634-56703b4acafe.gif",
    name: "咸水沽储能管理员",
    left: 5,
    number: ["1.5k", "2.8k", "55.2k"],
    earning: 75000
}

const warnings = [{
        content: "温度异常 40℃",
        addition: "咸水沽12站",
        tagType: "warning",
        tagContent: "二级警告",
        type: "temperature"
    },
    {
        content: "烟雾报警",
        addition: "天津大学2站",
        tagType: "danger",
        tagContent: "一级警告",
        type: "fog"
    },
    {
        content: "电压12.5V",
        addition: "西青区3站",
        tagType: "info",
        tagContent: "三级警告",
        type: "vv"
    }
]

const chargingRank = [{
        content: "天津大学2站",
        addition: "锂电池90串",
        tagType: "success",
        tagContent: "18.2K",
        email: "天津大学2站@dazto.com"
    },
    {
        content: "西青区3站",
        addition: "锌锰电池24串",
        tagType: "success",
        tagContent: "9.3K",
        email: "zzh_98@dazto.com"
    },
    {
        content: "咸水沽12站",
        addition: "碱性电池60串",
        tagType: "success",
        tagContent: "6.1K",
        email: "咸水沽12站@dazto.com"
    }
]

const dischargeRank = [{
        content: "咸水沽12站",
        addition: "碱性电池60串",
        tagType: "danger",
        tagContent: "6.1K",
        email: "咸水沽12站@dazto.com"
    }, {
        content: "天津大学2站",
        addition: "锂电池90串",
        tagType: "danger",
        tagContent: "18.2K",
        email: "天津大学2站@dazto.com"
    },
    {
        content: "西青区3站",
        addition: "锌锰电池24串",
        tagType: "danger",
        tagContent: "9.3K",
        email: "zzh_98@dazto.com"
    }
]

const stationTable = [
    {
        name: "咸水沽12站",
        user: "咸水沽储能管理员",
        position: [121.45, 124.5],
        deals: 109,
        delivered: 63,
        pending: 46,
        left: 37,
        status: "online"
    },{
        name: "天津大学2站",
        user: "咸水沽储能管理员",
        position: [151.76, 25.4],
        deals: 31,
        delivered: 29,
        pending: 5,
        left: 66,
        status: "offline"
    },{
        name: "西青区3站",
        user: "咸水沽储能管理员",
        position: [90.46, 72.53],
        deals: 18,
        delivered: 16,
        pending: 2,
        left: 55,
        status: "online"
    }
]

module.exports = [

    {
        url: '/dashboard/weekCharging',
        type: 'get',
        response: config => {
            return {
                code: 20000,
                data: LineData
            }
        }
    },

    {
        url: '/dashboard/batterySataus',
        type: 'get',
        response: config => {
            return {
                code: 20000,
                data: PieData
            }
        }
    },

    {
        url: '/dashboard/userInfo',
        type: 'get',
        response: config => {
            return {
                code: 20000,
                data: userInfo
            }
        }
    },

    {
        url: '/dashboard/warnings',
        type: 'get',
        response: config => {
            return {
                code: 20000,
                data: warnings
            }
        }
    },


    {
        url: '/dashboard/chargingRank',
        type: 'get',
        response: config => {
            return {
                code: 20000,
                data: chargingRank
            }
        }
    },
    {
        url: '/dashboard/dischangeRank',
        type: 'get',
        response: config => {
            return {
                code: 20000,
                data: dischargeRank
            }
        }
    },
    {
        url: '/dashboard/stations',
        type: 'get',
        response: config => {
            return {
                code: 20000,
                data: stationTable
            }
        }
    }

]