import { Component, OnInit } from '@angular/core';
import { faAngleDoubleUp } from '@fortawesome/free-solid-svg-icons';
import { faAngleDoubleDown } from '@fortawesome/free-solid-svg-icons';
import { faStop } from '@fortawesome/free-solid-svg-icons';
import { TelemetryService } from 'src/app/services/telemetry.service';
import { trigger, state, style, animate, transition } from '@angular/animations';

@Component({
    selector: 'app-buttons',
    templateUrl: './buttons.component.html',
    styleUrls: ['./buttons.component.scss'],
    animations: [],
})
export class ButtonsComponent implements OnInit {
    constructor(private telemetryService: TelemetryService) {}

    ngOnInit(): void {}

    faAngleDoubleUp = faAngleDoubleUp;
    faAngleDoubleDown = faAngleDoubleDown;
    faStop = faStop;

    speedIncrement: number = 10;
    speedMax: number = 100;
    speedMin: number = 0;

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
}
