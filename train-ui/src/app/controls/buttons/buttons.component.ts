import { Component, OnInit } from '@angular/core';
import { faAngleDoubleUp } from '@fortawesome/free-solid-svg-icons';
import { faAngleDoubleDown } from '@fortawesome/free-solid-svg-icons';
import { faStop } from '@fortawesome/free-solid-svg-icons';
import { TelemetryService } from 'src/app/services/telemetry.service';
import {
    trigger,
    state,
    style,
    animate,
    transition,
} from '@angular/animations';
import { Subscription } from 'rxjs';

@Component({
    selector: 'app-buttons',
    templateUrl: './buttons.component.html',
    styleUrls: ['./buttons.component.scss'],
    animations: [],
})
export class ButtonsComponent implements OnInit {
    speedIncrement: number = 10;
    speedMax: number = 100;
    speedMin: number = 0;
    headlightsOn: boolean = false;
    headlightSubscription!: Subscription;

    constructor(private telemetryService: TelemetryService) {}

    ngOnInit(): void {
        this.headlightSubscription = this.telemetryService.headlightsOn.subscribe(
            (headlightsOn) => {
                this.headlightsOn = headlightsOn;
                console.log(`Subscription headlightSubscription from ButtonsComponent ${headlightsOn}`);
            },
            (err) => {
                console.log(err);
            }
        );
    }

    faAngleDoubleUp = faAngleDoubleUp;
    faAngleDoubleDown = faAngleDoubleDown;
    faStop = faStop;

    setState(evt: Event, button: String) {
        evt.preventDefault();

        switch (button.toLowerCase()) {
            case 'forward': {
                console.log('forward action');
                this.telemetryService.increaseSpeed(this.speedIncrement);
                break;
            }
            case 'reverse': {
                console.log('reverse action');
                this.telemetryService.decreaseSpeed(this.speedIncrement);
                break;
            }
            case 'stop': {
                console.log('stop action');
                this.telemetryService.setSpeed(0);
                break;
            }
        }
    }

    toggleHeadlights(evt: Event) {
        evt.preventDefault();
        console.log('#DEBUG# buttons.component: headlight toggle action');
        this.headlightsOn = !this.headlightsOn;
        this.telemetryService.toggleHeadlights();
    }
}
