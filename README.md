# Portfolio

## 프로젝트 이름
Sword Master (가제)

## 작업 환경
Unreal Engine 4.22.1</br>
Visual Studio 2017

## 프로젝트 설명
보스와의 1:1 대결을 타이밍 액션을 통해 겨루는 게임 입니다.</br>
둥근원을 중심으로 반 시계 방향으로 회전하는 검과 시계 방향으로 회전하는 노트들이 서로 만나는 지점에서 적절한 액션을 수행하면 점수를 획득합니다.</br>

### 액션 - Attack
붉은 노트가 검과 교차될 때 마우스 왼쪽 클릭을 합니다.</br>

### 액션 - Guard
파란 노트가 검과 교차되기 전에 마우스를 클릭한 상태에서 노트가 지나가도록 합니다.</br>

### 판정
**Attack 노트**의 경우 검의 최대 범위의 30% 안에 들어오면 Excelent, 50% 안에 들어오면 Great, 전체 범위 안에 들어오면 Good 입니다. 판정을 할 때, 범위에 들어온 노트가 없다면 Miss 판정이 발생합니다. Excelent나 Great 판정을 받으면 검의 파워가 증가하여 더 높은 데미지를 보스에게 입힐 수 있습니다. 
단, Miss 판정을 기록하게 되면 검의 파워가 초기화 됩니다.</br>
**Guard 노트**의 경우에는 노트와 검이 교차할 때, 마우스 버튼이 눌려 있는 상태라면 Guard 판정이 발생하고, 눌려 있지 않았다면 Damaged 판정이 발생하여 플레이어 체력이 줄게 됩니다.</br>

### 게임의 흐름
Intro > Main Menu > Introducing Boss > Battle > Inter Stage > Introducing Boss > Battle > Inter Stage ... 로 기획하였나, 현재 구현은 한 개의 레벨 데이터만 넣어 뒀기 때문에 Battle 종료까지 확인할 수 있습니다. Inter Stage에서는 플레이어의 스탯을 업그레이드할 수 있는 메뉴를 준비할 예정 입니다.</br>

## 프로젝트 구조
### 클래스 설명
**ACameraDirector** - 레벨에 배치된 여러개의 카메라 사이를 일정 시간 간격으로 전환하여 보스를 여러가지 각도로 보여줍니다.</br>
**AGamePlayGameMode** - 인 게임 플레이 중 메뉴의 전환을 담당합니다.</br>
**AGamePlayGameState** - 인 게임 플레이 중 게임 승리, 패배를 판단하기 위한 데이터를 다룹니다.</br>
**AGamePlayPlayerController** - 이 클래스에서 실질적인 인 게임 진행을 담당합니다. 노트의 업데이트 및 히트 판정을 합니다.</br>
**USMGameInstance** - 전체 게임 플로우를 관리합니다. 전역적으로 사용될 데이터를 가지고 있습니다.</br>
**FLevelDataManager** - LevelData.xml 파일을 파싱해서 데이터를 보관합니다.</br>
### 블루프린트 클래스 설명
**BP_MainMenu** - Main Menu 플로우에서 보여줄 UI 입니다. 시작 버튼과 종료 버튼이 있습니다.</br>
**BP_GamePlay** - Battle 플로우에서 보여줄 UI 입니다. C++ 클래스로부터 데이터를 가져와서 화면에 표시합니다.</br>
