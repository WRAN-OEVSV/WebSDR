import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';

import { AppRoutingModule } from './app-routing.module';
import { AppComponent } from './app.component';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { NavbarComponent } from './navbar/navbar.component';
import { ClockComponent } from './clock/clock.component';
import { FooterComponent } from './footer/footer.component';
import {MatToolbarModule} from "@angular/material/toolbar";
import {MatButtonModule} from "@angular/material/button";
import {MatMenuModule} from "@angular/material/menu";
import {MatIconModule} from "@angular/material/icon";
import {MatSidenavModule} from "@angular/material/sidenav";
import { WaterfallComponent } from './waterfall/waterfall.component';
import { FlexLayoutModule } from '@angular/flex-layout';
import { UserManagementComponent } from './user-management/user-management.component';
import { LoginFormComponent } from './user-management/login-form/login-form.component';
import {MatInputModule} from "@angular/material/input";
import {MatDialogModule} from '@angular/material/dialog';
import { LogViewComponent } from './log-view/log-view.component';
import { NeighbourCacheComponent } from './neighbour-cache/neighbour-cache.component';
import { CopyToClipboradTextWithBtnComponent } from './copy-to-clipborad-text-with-btn/copy-to-clipborad-text-with-btn.component';

@NgModule({
  declarations: [
    AppComponent,
    NavbarComponent,
    ClockComponent,
    FooterComponent,
    WaterfallComponent,
    UserManagementComponent,
    LoginFormComponent,
    LogViewComponent,
    NeighbourCacheComponent,
    CopyToClipboradTextWithBtnComponent
  ],
    imports: [
        BrowserModule,
        AppRoutingModule,
        BrowserAnimationsModule,
        FlexLayoutModule,
        MatToolbarModule,
        MatButtonModule,
        MatMenuModule,
        MatIconModule,
        MatSidenavModule,
        MatInputModule,
        MatDialogModule
    ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
