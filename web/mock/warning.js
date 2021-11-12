const WeekData = {
    xAxis: ["周一", "周二", "周三", "周四", "周五", "周六", "周日"],
    curWeek: [10, 70, 60, 80, 70, 10, 80],
    lastWeek: [20, 40, 10, 60, 50, 10, 60]
}

const TypeWeekData = {
    xAxis: ["周一", "周二", "周三", "周四", "周五", "周六", "周日"],
    temp: [320, 302, 301, 334, 390, 330, 320],
    voltage: [120, 132, 101, 134, 90, 230, 210],
    smog: [220, 182, 191, 234, 290, 330, 310],
}

const StatusData = [
    { name: '已处理', value: 30 },
    { name: '未处理', value: 30 },
]

const TypeData = {
    "温度": 30,
    "电压": 12,
    "烟雾": 20
}

const BmuData = [
    { name: '1', value: 10 },
    { name: '2', value: 20 },
    { name: '3', value: 30 },
    { name: '4', value: 30 },
    { name: '5', value: 30 },
    { name: '6', value: 30 },
    { name: '7', value: 30 },
    { name: '8', value: 30 },
    { name: '9', value: 30 },
    { name: '10', value: 30 },
]

const warningTable = [
    {
        id: '00000000000000000001',
        pointid: 23,
        type: "温度过高",
        status: "已处理",
        value: "56",
        solver: "1234567891",
        solution: "解决方式",
        starttime: '2021-06-12 15:43',
        endtime: '2021-06-12 15:43',
        bsu: '1号',
        bcu: '1号',
        bmu: '1号'
    }, {
        id: '00000000000000000002',
        pointid: 12,
        type: "温度过高",
        status: "未处理",
        value: "45",
        solver: "1234567891",
        solution: "解决方式",
        starttime: '2021-06-12 15:43',
        endtime: '2021-06-12 15:43',
        bsu: '2号',
        bcu: '2号',
        bmu: '2号'
    },{
        id: '00000000000000000003',
        pointid: 34,
        type: "电压过高",
        status: "未处理",
        value: "56",
        solver: "1234567891",
        solution: "解决方式",
        starttime: '2021-06-12 15:43',
        endtime: '2021-06-12 15:43',
        bsu: '3号',
        bcu: '3号',
        bmu: '3号'
    }
]

module.exports = [


    {
        url: '/warning/warningWeek',
        type: 'get',
        response: config => {
            return {
                code: 20000,
                data: WeekData
            }
        }
    },

    {
        url: '/warning/warningTypeWeek',
        type: 'get',
        response: config => {
            return {
                code: 20000,
                data: TypeWeekData
            }
        }
    },

    {
        url: '/warning/warningSataus',
        type: 'get',
        response: config => {
            return {
                code: 20000,
                data: StatusData
            }
        }
    },

    {
        url: '/warning/warningType',
        type: 'get',
        response: config => {
            return {
                code: 20000,
                data: TypeData
            }
        }
    },

    {
        url: '/warning/warningBmu',
        type: 'get',
        response: config => {
            return {
                code: 20000,
                data: BmuData
            }
        }
    },

    {
        url: '/warning/warnings',
        type: 'get',
        response: config => {
            return {
                code: 20000,
                data: warningTable
            }
        }
    },

]