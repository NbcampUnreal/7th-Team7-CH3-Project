# 🔫 T-DAS (Top-Down Action Shooter)
> **데이터 기반 설계와 확장 가능한 C++ 아키텍처를 지향하는 UE5 탑다운 슈팅 액션 게임**

<div align="center">
  <img src="https://img1.daumcdn.net/thumb/R1280x0/?scode=mtistory2&fname=https%3A%2F%2Fblog.kakaocdn.net%2Fdna%2Fbn8DCO%2FdJMcadOCPh2%2FAAAAAAAAAAAAAAAAAAAAAItXAiogKo1g1mwgiELQpFcss4O910v8tkGEH5z-VVGh%2Fimg.png%3Fcredential%3DyqXZFxpELC7KVnFOS48ylbz2pIh7yKj8%26expires%3D1774969199%26allow_ip%3D%26allow_referer%3D%26signature%3DY24TI8%252FAhVHmxcl%252BWv8CRSB2%252BnE%253D" width="100%" alt="T-DAS Main Banner">

  <br>
  <br>
  
  ![Unreal Engine 5](https://img.shields.io/badge/Unreal%20Engine-5.5.4-white?style=flat-square&logo=unrealengine&logoColor=black)
  ![C++](https://img.shields.io/badge/C++-17-00599C?style=flat-square&logo=c%2B%2B&logoColor=white)
  ![Platform](https://img.shields.io/badge/Platform-PC-EF9421?style=flat-square)
  ![License](https://img.shields.io/badge/License-MIT-green?style=flat-square)

  [🎥**시연 영상**](https://www.youtube.com/watch?v=ysVfI3eZeFc) | 
  [📄**최종 기획서**](https://www.notion.so/T-DAS-318711d71de1805e9cdcf1b62a7ee629) | 
  [📊**발표 자료**](https://www.canva.com/design/DAHC9vTyW_A/Xb2PWGMd33L6fJ2DeabAsA/edit) | 
  [📦**게임 패키징**](https://drive.google.com/file/d/1VdlQZ1GdJNZyTtV87XEx1aHyIOUaY_5M/view) |
  [💻**저장소**](https://github.com/NbcampUnreal/7th-Team7-CH3-Project)

</div>

---

## 🚀 1. 프로젝트 개요
T-DAS는 점진적으로 난이도가 상승하는 웨이브 시스템 속에서 무기를 해금하고 전략적인 전투를 수행하는 싱글 플레이 탑다운 슈터입니다.

* **핵심 가치**: 단순 기능 구현을 넘어, **유지보수가 용이한 이벤트 드리븐 UI 구조**와 **데이터 테이블 중심의 밸런싱 시스템** 구축에 집중했습니다.
* **장르 / 엔진**: 탑다운 액션 슈터 / Unreal Engine 5.5.4 (C++)
* **주요 포인트**: 확장 가능한 전투 루프 및 난이도 스케일링 수식 설계

---

## 🛠 2. 기술 아키텍처 (Technical Architecture)

### 2-1. 의존성 구조 (Dependency Bus)
객체 간 결합도를 낮추기 위해 `DevHUISubSystem`을 메시지 버스로 사용하는 **이벤트 드리븐 구조**를 채택했습니다.
- **SubSystem**: HP, 스태미나 등 데이터 변화를 델리게이트로 방송하여 로직과 UI를 완전히 분리했습니다.
- **Data-Driven Design**: 적 스탯, 무기 속성, 스테이지 구성을 `DataTable`로 관리하여 생산성을 높였습니다.

### 2-2. 난이도 스케일링 (Scaling Formula)
플레이어 성장에 맞춰 적의 스탯이 정교하게 보정되도록 수식을 설계했습니다.
> $$FinalHealth = BaseHealth \times (1 + StageHealthInc \times Stage + WaveHealthInc \times Wave)$$

---

## 🎮 3. 핵심 시스템 (Core Systems)

### 3-1. 스테이지 & 웨이브 루프
- **AStageSpawner**: NavMesh 기반 랜덤 위치 스폰 및 플레이어 시야 외부 우선순위 로직 적용.
- **Gameplay Loop**: Stage 로드 → 웨이브 전투 → 점수 획득 → 무기 해금 → 스테이지 클리어.

### 3-2. 전투 및 기동 시스템
- **듀얼 무기**: 총기 사격(LineTrace)과 수류탄(Projectile)의 즉각적인 연계 전투.
- **수류탄 로직**: 포물선 궤적 계산 및 폭발 반경 내 거리별 데미지 감쇄(Radial Damage) 구현.
- **전략적 기동**: 스태미나를 소모하는 **0.5초 무적 대시**와 스프린트로 생존성 확보.

---

## 🎬 Gameplay Preview
<div align="center">
  <a href="https://www.youtube.com/watch?v=ysVfI3eZeFc">
    <img src="https://img.youtube.com/vi/ysVfI3eZeFc/maxresdefault.jpg" width="85%" style="border-radius: 10px;">
    <br>
    <sub><b>클릭 시 YouTube 시연 영상으로 이동합니다</b></sub>
  </a>
</div>

---

## 📊 4. 데이터 테이블 명세 (Data Snippet)
프로젝트의 확장성을 위해 다양한 게임 데이터를 `UDataTable` 구조체로 정의하여 관리합니다.

| 카테고리 | 핵심 필드 | 역할 및 활용 |
| :--- | :--- | :--- |
| **Enemy** | `AttackType`, `WaveHealthInc` | AI 행동 트리 분기점 및 성장 계수 결정 |
| **Weapon** | `PelletCount`, `UnlockScore` | 산탄 발사 수 제어 및 점수 기반 해금 트리거 |
| **Player** | `StaminaRecoveryRate`, `SprintSpeed` | 플레이어 기동 편의성 및 자원 소모 밸런싱 |
| **Stage** | `SpawnWaves`, `WavesDelay` | 스테이지별 난이도 구성 및 웨이브 템포 조절 |

---

## ⌨️ 5. 컨트롤 가이드 (Controls)
**Enhanced Input** 시스템을 활용하여 부드러운 조작감을 구현했습니다.

| 입력 | 동작 | 상세 설명 |
| :--- | :--- | :--- |
| **WASD** | 이동 | 캐릭터 이동 (후진 시 속도 0.6배 페널티 적용) |
| **Mouse Move** | 조준 | 마우스 커서 방향 레이캐스트 기반 캐릭터 실시간 회전 |
| **L-Click** | 총기 사격 | 무기 컴포넌트 데이터 기반 라인트레이스 판정 |
| **R-Click** | 수류탄 투척 | 투사체 발사 및 범위 폭발 데미지 실행 |
| **Shift / Space** | 스프린트 / 대시 | 스태미나 소비 기반 고유 이동기 (대시 시 0.5초 무적) |

---

## ⚙️ 6. 기술 설정 (Tech Specs)
- **Rendering**: Lumen GI, Lumen Reflections, DirectX 12 (SM6)
- **Audio**: 48,000Hz 샘플레이트 기반 오디오 믹싱
- **Modules**: NavigationSystem, AIModule, Niagara, EnhancedInput, MediaAssets

---
</div>
