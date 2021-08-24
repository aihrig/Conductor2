import { Component, OnInit, Input } from '@angular/core';
import { EChartsOption } from 'echarts';
import { Subscription } from 'rxjs';
import { TelemetryService } from 'src/app/services/telemetry.service';

@Component({
    selector: 'app-speedometer',
    templateUrl: './speedometer.component.html',
    styleUrls: ['./speedometer.component.scss'],
})
export class SpeedometerComponent implements OnInit {
    @Input()
    currentSpeed!: String;
    subscription!: Subscription;

    speedChartOptions!: EChartsOption;

    constructor(private telemetryService: TelemetryService) {}

    ngOnInit(): void {
        this.subscription = this.telemetryService.currentSpeed.subscribe(
            (speed) => {
                this.currentSpeed = String(speed);
                this.setOptions();
                console.log(
                    `Subscription speed from SpeedometerComponent ${speed}`
                );
            },
            err => {
                console.log(err);
            }
        );
    }

    setOptions() {
        this.speedChartOptions = {
            series: [
                {
                    type: 'gauge',
                    startAngle: 200,
                    endAngle: -20,
                    min: 0,
                    max: 100,
                    splitNumber: 10,
                    itemStyle: {
                        color: '#de1f1f',
                        shadowColor: 'rgba(0,138,255,0.45)',
                        shadowBlur: 10,
                        shadowOffsetX: 2,
                        shadowOffsetY: 2,
                    },
                    progress: {
                        show: true,
                        roundCap: true,
                        width: 6,
                    },
                    pointer: {
                        icon: 'path://M2090.36389,615.30999 L2090.36389,615.30999 C2091.48372,615.30999 2092.40383,616.194028 2092.44859,617.312956 L2096.90698,728.755929 C2097.05155,732.369577 2094.2393,735.416212 2090.62566,735.56078 C2090.53845,735.564269 2090.45117,735.566014 2090.36389,735.566014 L2090.36389,735.566014 C2086.74736,735.566014 2083.81557,732.63423 2083.81557,729.017692 C2083.81557,728.930412 2083.81732,728.84314 2083.82081,728.755929 L2088.2792,617.312956 C2088.32396,616.194028 2089.24407,615.30999 2090.36389,615.30999 Z',
                        length: '75%',
                        width: 14,
                        offsetCenter: [0, '5%'],
                    },
                    axisLine: {
                        roundCap: true,
                        lineStyle: {
                            width: 3,
                            color: [[1, '#E6EBF8']],
                        },
                    },
                    axisTick: {
                        splitNumber: 2,
                        lineStyle: {
                            width: 2,
                            color: '#999',
                        },
                    },
                    splitLine: {
                        length: 12,
                        lineStyle: {
                            width: 3,
                            color: '#999',
                        },
                    },
                    axisLabel: {
                        distance: 18,
                        color: '#999',
                        fontSize: 13,
                    },
                    title: {
                        show: false,
                    },
                    detail: {
                        padding: [0, 10],
                        backgroundColor: '#fff',
                        borderColor: '#999',
                        borderWidth: 2,
                        width: '75%',
                        lineHeight: 40,
                        height: 40,
                        borderRadius: 8,
                        offsetCenter: [0, '35%'],
                        valueAnimation: true,
                        formatter: function (value) {
                            return (
                                '{value|' + value.toFixed(0) + '}{unit|km/h}'
                            );
                        },
                        rich: {
                            value: {
                                fontSize: 34,
                                fontWeight: 'bolder',
                                color: '#777',
                            },
                            unit: {
                                fontSize: 16,
                                color: '#999',
                                padding: [0, 0, -20, 10],
                            },
                        },
                    },
                    data: [{ 'value': Number(this.currentSpeed)}],
                },
            ],
        };
    }

}
