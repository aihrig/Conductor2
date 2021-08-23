import { Injectable, OnInit } from '@angular/core';
import { PubNubAngular } from 'pubnub-angular2';
import { BehaviorSubject, Observable, of } from 'rxjs';
import { Telemetry } from '../interfaces/telemetry';

@Injectable({
    providedIn: 'root',
})
export class TelemetryService implements OnInit {
    pubnub!: PubNubAngular;
    channel: String;
    // currentSpeed: Number = 0.1;

    private messageSource = new BehaviorSubject('default message');
    currentMessage = this.messageSource.asObservable();

    private speedSource = new BehaviorSubject(0.1);
    currentSpeed = this.speedSource.asObservable();

    constructor(pubnub: PubNubAngular) {
        this.channel = 'hst01';
        var _this = this;
        pubnub.init({
            publishKey: 'pub-c-444d9350-c274-45cd-ab65-007aba298db9',
            subscribeKey: 'sub-c-17598d96-f23b-11e6-b7fd-02ee2ddab7fe',
        });

        pubnub.addListener({
            status: function (st: {
                category: string;
                errorData: { message: any };
            }) {
                if (st.category === 'PNUnknownCategory') {
                    var newState = {
                        new: 'error',
                    };
                    pubnub.setState(
                        {
                            state: newState,
                        },
                        function (status) {
                            console.log(st.errorData.message);
                        }
                    );
                }
            },
            message: function (message: any) {
                _this.handleMessage(message);
            },
        });

        pubnub.subscribe({
            channels: [this.channel],
            triggerEvents: ['message'],
        });
    }

    telemetryData!: Telemetry;

    ngOnInit() {}

    getSpeed(): Observable<any> {
        return of(this.currentSpeed);
    }

    private handleMessage(message: any) {
        // this.currentSpeed = message.message[0].status.speed;
        // console.log('New current speed: ' + this.currentSpeed);
        console.log(`Message: ${JSON.stringify(message)}`);
        console.log(`Speed: ${message.message[0].status.speed}`);

        this.speedSource.next(message.message[0].status.speed);        

    }
}
