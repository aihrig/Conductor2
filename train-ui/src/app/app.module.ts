import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';

import { AppRoutingModule } from './app-routing.module';
import { AppComponent } from './app.component';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { SpeedometerComponent } from './controls/speedometer/speedometer.component';
import { NgxEchartsModule } from 'ngx-echarts';
import { ButtonsComponent } from './controls/buttons/buttons.component';
import { FontAwesomeModule } from '@fortawesome/angular-fontawesome';
import { NgbModule } from '@ng-bootstrap/ng-bootstrap';
import { TelemetryService } from './services/telemetry.service';
@NgModule({
    declarations: [AppComponent, SpeedometerComponent, ButtonsComponent],
    imports: [
        BrowserModule,
        AppRoutingModule,
        BrowserAnimationsModule,
        NgxEchartsModule.forRoot({
            echarts: () => import('echarts'),
        }),
        FontAwesomeModule,
        NgbModule,
    ],
    providers: [TelemetryService],
    bootstrap: [AppComponent],
})
export class AppModule {}
